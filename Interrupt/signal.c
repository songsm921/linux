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
#include <linux/sched.h>
#define TIMER_ADDR_BASE 0xA0000000
#define TIMER_ADDR_HIGH 0xA000FFFF
#define TIMER_DOWN_COUNT_OFFSET 0x00000002
#define TIMER_LOAD_REG_OFFSET 0x00000004
#define TIMER_VALUE 0x000186A0
#define TIMER_MASK_AUTO_RELOAD 0x00000010
#define TIMER_MASK_LOAD_ENABLE 0x00000020
#define TIMER_MASK_INTR_ENABLE 0x00000040
#define TIMER_MASK_START 0x00000080
#define TIMER_MASK_CLEAR_INTR 0x00000100
#define DEVICE_NAME "timer"
#define MINOR_BASE 0
static unsigned int IRQNum;
static dev_t my_dev;
static struct cdev my_cdev;
static struct class *my_class;
static unsigned int irqnum = 49; // or 52? Finally, 49
static int process_pid;
unsigned int prev;
void* timer_addr; 
struct task_struct *my_task;
static irq_handler_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){ 
    //prev = ioread32(timer_addr);
    //iowrite32(prev|TIMER_MASK_CLEAR_INTR,timer_addr);
    printk(KERN_ALERT "Timer interrupted! \n");
    send_sig(SIGUSR1,my_task,0);
    printk(KERN_ALERT "Signal was sent from Kernel! \n");
    return (irq_handler_t) IRQ_HANDLED;
}
static ssize_t timer_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
    int ret;
    unsigned int val;
    if(count != sizeof(unsigned int)){
        printk(KERN_ALERT "Invalid size of data\n");
        return -EINVAL;
    }
    ret = copy_from_user(&val, buf, count);
    if(ret < 0){
        printk(KERN_ALERT "copy_from_user failed\n");
        return -EFAULT;
    }
    printk(KERN_INFO "Process ID is %d\n", val);
    process_pid = val;
    my_task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    prev = ioread32(timer_addr);
    iowrite32(prev|TIMER_MASK_START,timer_addr);
    return count;
}
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .write = timer_write,
};
static int timer_init(void){
    /* Device driver Registration */
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
    request_irq(irqnum, (irq_handler_t)irq_handler, IRQF_TRIGGER_RISING, "timer_interrupt", NULL);
    /* Timer Setting */
    timer_addr  = ioremap(TIMER_ADDR_BASE,TIMER_ADDR_HIGH-TIMER_ADDR_BASE+1);
    if(timer_addr == NULL){
        printk(KERN_ERR "ioremap failed\n");
        return -1;
    }
    iowrite32(TIMER_VALUE,timer_addr+TIMER_LOAD_REG_OFFSET);
    /*Timer value setting*/
    iowrite32(TIMER_MASK_LOAD_ENABLE,timer_addr);
    /*Timer load enable, it should be clear.*/
    iowrite32(TIMER_MASK_INTR_ENABLE | TIMER_MASK_AUTO_RELOAD | TIMER_DOWN_COUNT_OFFSET,timer_addr);

    /*Timer interrupt enable & auto reload, then timer start.*/
    return 0;
}
static void timer_exit(void){
    //iowrite32(0,timer_addr);
    free_irq(irqnum, NULL);
    iounmap(timer_addr);
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
}
module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
