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
#define LEN 5565
#define MINOR_BASE 0
#define DEVICENAME "LED"
volatile unsigned char* gpio; // AXI GPIO Base Address
static dev_t my_dev; // major # && minor #
static struct class *my_class;
static struct cdev my_cdev;


static int led_driver_open(struct inode* inode, struct file* file){
   
    gpio = (char *)ioremap(GPIO_ADDRESS,LEN);
    return 0;
}
static ssize_t led_driver_write(struct file* file, const char __user* buf, size_t count, loff_t* ppos){
    int val;
    get_user(val, (int *)buf);
    *gpio = val;
    return 0;
}
static ssize_t led_driver_read(struct file* file, char __user* buf, size_t count, loff_t* ppos){
    copy_to_user(buf,gpio,1);
    return 0;
}
static int led_driver_release(struct inode *inode, struct file *file){
    iounmap(gpio);
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
    alloc_chrdev_region(&my_dev,MINOR_BASE,1,DEVICENAME);
    cdev_init(&my_cdev,&led_driver_fops);
    cdev_add(&my_cdev,my_dev,1);
    //register_chrdev(345,DEVICENAME,&led_driver_fops);
    my_class = class_create(THIS_MODULE,DEVICENAME); // /sys/class
    device_create(my_class,NULL,my_dev,NULL,DEVICENAME); // dev/NAME
    //gpio = (char *)ioremap(GPIO_ADDRESS,LEN);
    return 0;
}

static void led_driver_exit(void){
    device_destroy(my_class,my_dev);
    class_destroy(my_class);
    unregister_chrdev_region(my_dev,1);
    //iounmap(gpio);
    printk(KERN_INFO "LED module out\n");
}



module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL");