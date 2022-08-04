#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/unistd.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/device.h>
#define GPIO_BASE 504
#define DEVICE_NAME "LED"
#define MINOR_BASE 0
static dev_t my_dev;
static struct cdev my_cdev;
static struct class *my_class;
static int LED_open(struct inode *inode, struct file *file){
    printk(KERN_INFO "LED_open\n");
    return 0;
}
static int LED_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "LED_release\n");
    return 0;
}
static ssize_t LED_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
    printk(KERN_INFO "LED_write\n");
    if(gpio_get_value(GPIO_BASE) == 1)
        gpio_set_value(GPIO_BASE, 0);
    mdelay(1); // For calculation
    gpio_set_value(GPIO_BASE,1);
    return 0;
}
static ssize_t LED_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos){
    printk(KERN_INFO "LED_read\n");
    int value = gpio_get_value(GPIO_BASE);
    copy_to_user(buf, &value, sizeof(int));
    return 0;
}
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = LED_open,
    .release = LED_release,
    .write = LED_write,
    .read = LED_read,
};
static int LED_init(void){
    int ret;
    printk(KERN_INFO "LED_init\n");
    gpio_request(GPIO_BASE,"sysfs");
    gpio_direction_output(GPIO_BASE,0);
    gpio_export(GPIO_BASE,false);
    ret = alloc_chrdev_region(&my_dev, MINOR_BASE, 1, DEVICE_NAME);
    if(ret < 0){
        printk(KERN_ERR "alloc_chrdev_region failed\n");
        return ret;
    }
    cdev_init(&my_cdev, &my_fops);
    ret = cdev_add(&my_cdev, my_dev, 1);
    if(ret < 0){
        printk(KERN_ERR "cdev_add failed\n");
        return ret;
    }
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(my_class)){
        printk(KERN_ERR "class_create failed\n");
        return PTR_ERR(my_class);
    }
    device_create(my_class, NULL, my_dev, NULL, DEVICE_NAME);
    return 0;
}
static void LED_exit(void){
    printk(KERN_INFO "LED_exit\n");
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    gpio_unexport(GPIO_BASE);
    gpio_free(GPIO_BASE);
}
module_init(LED_init);
module_exit(LED_exit);
MODULE_LICENSE("GPL");