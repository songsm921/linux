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
#define DEVICENAME "poll"
#define MINOR_BASE 0
MODULE_LICENSE("GPL");
static dev_t my_dev; // major # && minor #
static struct class *my_class;
static struct cdev my_cdev;
static unsigned int gpio_button = 507;
static unsigned int gpio_led = 499;
static unsigned int irqNum;
static bool ledValue = 0;
static bool buttonstatus;
/*static irq_handler_t button(unsigned int irq, void *dev_id, struct pt_regs *regs){
    printk(KERN_ALERT "Button pressed\n");
    ledValue = !ledValue;
    gpio_set_value(gpio_led,ledValue);
    return (irq_handler_t) IRQ_HANDLED;
}*/
static int button_open(struct inode *inode, struct file *file){
    printk(KERN_INFO "Driver opened\n");
    return 0;
}
static int button_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Driver closed\n");
    return 0;
}
static ssize_t button_read(struct file* file, char* buf, size_t count, loff_t* offset){
    buttonstatus = gpio_get_value(gpio_button);
    copy_to_user(buf,&buttonstatus,sizeof(buttonstatus));
    return 0;
}
static ssize_t button_write(struct file* file, const char* buf, size_t count, loff_t* offset){
    ledValue = !ledValue;
    gpio_set_value(gpio_led,ledValue);
    return 0;
}
static struct file_operations led_driver_fops = {
    .owner = THIS_MODULE,
    .open = button_open,
    .write = button_write,
    .read = button_read,
    .release = button_release,
};
static int button_init(void){
    int result;
    alloc_chrdev_region(&my_dev,MINOR_BASE,1,DEVICENAME);
    cdev_init(&my_cdev,&led_driver_fops);
    cdev_add(&my_cdev,my_dev,1);
    my_class = class_create(THIS_MODULE,DEVICENAME); // /sys/class
    device_create(my_class,NULL,my_dev,NULL,DEVICENAME); // dev/NAME
    gpio_request(gpio_led,"sysfs");
    gpio_direction_output(gpio_led,ledValue);
    gpio_export(gpio_led,false);
    gpio_request(gpio_button,"sysfs");
    gpio_direction_input(gpio_button);
    gpio_export(gpio_button,false);
    printk(KERN_INFO"GPIO_TEST : The button state is currently %d\n",gpio_get_value(gpio_button));
    //irqNum = gpio_to_irq(gpio_button);
    //printk(KERN_INFO"GPIO_TEST : The button is mapped to IRQ %d\n",irqNum);
    //result = request_irq(irqNum,(irq_handler_t)button,IRQF_TRIGGER_RISING,"switch",NULL);
   /* if(result < 0){
        printk(KERN_INFO"GPIO_TEST : Failed to request IRQ %d\n", result);
        return result;
    }*/
    return 0;
}
static void button_exit(void){
    //free_irq(irqNum,NULL);
    gpio_unexport(gpio_button);
    gpio_free(gpio_button);
    gpio_unexport(gpio_led);
    gpio_free(gpio_led);
    device_destroy(my_class,my_dev);
    class_destroy(my_class);
    unregister_chrdev_region(my_dev,1);
}
module_init(button_init);
module_exit(button_exit);
