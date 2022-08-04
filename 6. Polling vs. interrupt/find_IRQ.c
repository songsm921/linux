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
#define DEVICE_NAME "timer"
#define MINOR_BASE 0
static dev_t my_dev;
static struct cdev my_cdev;
static struct class *my_class;
static unsigned int irqNum;
static irq_handler_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
    printk(KERN_ALERT "Timer interrupted! \n");
    return (irq_handler_t) IRQ_HANDLED;
}
static ssize_t find_IRQ_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
    int iRQ;
    copy_from_user(&iRQ, buf, sizeof(int));
    int ret = request_irq(iRQ,(irq_handler_t)irq_handler,IRQF_TRIGGER_RISING,"timer",NULL);
    if(ret < 0)
    {
        printk(KERN_INFO"%d is not proper IRQ # \n", iRQ);
    }
    else
    {
        printk(KERN_INFO"%d is proper IRQ # \n", iRQ);
    }
    return ret;
}
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .write = find_IRQ_write,
};
static int find_IRQ_init(void){
    int ret;
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
static void find_IRQ_exit(void){
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    free_irq(irqNum,NULL);
}
module_init(find_IRQ_init);
module_exit(find_IRQ_exit);
MODULE_LICENSE("GPL");