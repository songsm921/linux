#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/cdev.h>

#define MINOR_BASE 0
#define DEVICE_NAME "test"

static dev_t my_dev;
static struct class *my_class;
static struct cdev my_cdev;

static struct file_operations fops ={

};
int __init device_init(void){
    if(alloc_chrdev_region(&my_dev,MINOR_BASE,1,DEVICE_NAME)){
        printk(KERN_ALERT"[%s]ADD FAILED\n",__func__);
        goto err_return;
    }
    cdev_init(&my_cdev,&fops);
    if(cdev_add(&my_cdev,my_dev,1)){
        printk(KERN_ALERT "[%s] cdev_add failed\n", __func__);
		goto unreg_device;
    }
    if((my_class = class_create(THIS_MODULE,DEVICE_NAME)) == NULL){
        printk(KERN_ALERT"[%s]class ADD failed\n",__func__);
        goto unreg_device;
    }
    if(device_create(my_class,NULL,my_dev,NULL,DEVICE_NAME) == NULL)
    {
        goto unreg_class;
    }
    printk(KERN_ALERT"[%s] successfully created device major = %d, minor = %d",__func__,MAJOR(my_dev),MINOR(my_dev));
    return 0;

    unreg_class:
        class_destroy(my_class);
    unreg_device:
        unregister_chrdev_region(my_dev,1);
    err_return:
        return -1;
}
void __exit device_exit(void){
    device_destroy(my_class,my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev,1);
    printk(KERN_ALERT"[%s] successfully unregistered",__func__);
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");