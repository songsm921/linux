#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/unistd.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#define DEVICE_NAME "LED"
#define MINOR_BASE 0
static dev_t my_dev;
static struct cdev my_cdev;
static struct class *my_class;
static unsigned int gpio_led = 507;
static unsigned int irqNum;
static bool ledValue = 0;
static irq_handler_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
    printk(KERN_ALERT "LED ON interrupted!\n");
    return (irq_handler_t) IRQ_HANDLED;
}
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
    ledValue = !ledValue;  
    gpio_set_value(gpio_led, ledValue);
    return 0;
}
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = LED_open,
    .release = LED_release,
    .write = LED_write,
};
static int LED_init(void){
    int result;
    int ret;
    gpio_request(gpio_led,"sysfs");
    gpio_direction_input(gpio_led);
    gpio_export(gpio_led,false);
    irqNum = gpio_to_irq(gpio_led);
    printk(KERN_INFO"GPIO_TEST : The button is mapped to IRQ %d\n",irqNum);
    result = request_irq(irqNum,(irq_handler_t)irq_handler,IRQF_TRIGGER_RISING,"led",NULL);
    if(result < 0){
        printk(KERN_INFO"GPIO_TEST : Failed to request IRQ %d\n", result);
        return result;
    }
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
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    free_irq(irqNum,NULL);
    gpio_unexport(gpio_led);
    gpio_free(gpio_led);
}
module_init(LED_init);
module_exit(LED_exit);
MODULE_LICENSE("GPL");