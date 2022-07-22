#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define GPIO_ADDRESS 0x00A0000000
#define LEN 1
#define MINOR_BASE 0
#define DEVICENAME "LED"
volatile unsigned char* gpio; // AXI GPIO Base Address
static dev_t my_dev; // major # && minor #
static struct class *my_class;
static struct cdev my_cdev;


static int led_driver_open(struct inode* inode, struct file* file){
    printk(KERN_INFO "Dev file opened \n");
    return 0;
}
static ssize_t led_driver_write(struct file* file, const char __user* buf, size_t count, loff_t* ppos){
    int val;
    if(get_user(val, buf) != 0){
        printk(KERN_ALERT "[%s] get_user FAILED!\n",__func__);
        return 101;
    }
    printk(KERN_INFO "[%s] User value %d. \n",__func__,val);
    *gpio = val;
    return 0;
}
static ssize_t led_driver_read(struct file* file, char __user* buf, size_t count, loff_t* ppos){
    if(copy_to_user(buf,gpio,sizeof(unsigned char))!=0){
        printk(KERN_ALERT "[%s] copy_to_user is FAILED! \n",__func__);
        return 102;
    }
    return 0;
}
static int led_driver_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Dev file closed \n");
    return 0;
}
static struct file_operations led_driver_fops = {
    .owner = THIS_MODULE,
    .open = led_driver_open,
    .write = led_driver_write,
    .read = led_driver_read,
    .release = led_driver_release,
};

static int led_driver_init(void)
{
    printk(KERN_INFO "Driver register start\n");
    if(alloc_chrdev_region(&my_dev,MINOR_BASE,1,DEVICENAME) < 0)
    {
        printk(KERN_ALERT "[%s] alloc_chrdev_region FAILED \n",__func__);
        return 103;
    }
    cdev_init(&my_cdev,&led_driver_fops);
    if(cdev_add(&my_cdev,my_dev,1) < 0)
    {
        printk(KERN_ALERT "[%s] cdev_add FAILED \n",__func__);
        return 104;
    }
    //register_chrdev(345,DEVICENAME,&led_driver_fops);
    my_class = class_create(THIS_MODULE,DEVICENAME); // /sys/class
    if(IS_ERR(my_class))
    {
        printk(KERN_ALERT "[%s] class_create FAILED \n",__func__);
        return 105;
    }
    if(IS_ERR(device_create(my_class,NULL,my_dev,NULL,DEVICENAME))) // dev/NAME
    {
        printk(KERN_ALERT "[%s] device_create FAILED \n",__func__);
        return 105;
    }
    gpio = (unsigned char *)ioremap(GPIO_ADDRESS,LEN);
    if(gpio == NULL)
    {
        printk(KERN_ALERT "[%s] ioreamp FAILED!\n",__func__);
        return 100;
    }
    printk(KERN_INFO "[%s] GPIO %x. \n",__func__,(unsigned int)gpio);
    printk(KERN_INFO "Driver register finished succssfully \n");
    return 0;
}

static void led_driver_exit(void){
    device_destroy(my_class,my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev,1);
    iounmap(gpio);
    printk(KERN_INFO "LED module out\n");
}
module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL");