#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
static unsigned int gpio_button = 507;
static unsigned int gpio_led = 499;
static unsigned int irqNum;
static bool ledValue = 0;
static irq_handler_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
    printk(KERN_ALERT "Button pressed\n");
    ledValue = !ledValue;
    gpio_set_value(gpio_led,ledValue);
    return (irq_handler_t) IRQ_HANDLED;
}
static int button_init(void){
    int result;
    gpio_request(gpio_led,"sysfs");
    gpio_direction_output(gpio_led,ledValue);
    gpio_export(gpio_led,false);
    gpio_request(gpio_button,"sysfs");
    gpio_direction_input(gpio_button);
    gpio_export(gpio_button,false);
    printk(KERN_INFO"GPIO_TEST : The button state is currently %d\n",gpio_get_value(gpio_button));
    irqNum = gpio_to_irq(gpio_button);
    printk(KERN_INFO"GPIO_TEST : The button is mapped to IRQ %d\n",irqNum);
    result = request_irq(irqNum,(irq_handler_t)irq_handler,IRQF_TRIGGER_RISING,"switch",NULL);
    if(result < 0){
        printk(KERN_INFO"GPIO_TEST : Failed to request IRQ %d\n", result);
        return result;
    }
    return 0;
}
static void button_exit(void){
    free_irq(irqNum,NULL);
    gpio_unexport(gpio_button);
    gpio_free(gpio_button);
    gpio_unexport(gpio_led);
    gpio_free(gpio_led);
}
module_init(button_init);
module_exit(button_exit);
