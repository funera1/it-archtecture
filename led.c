#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/gpio.h>


static int br;
static int col;
static int flag;
module_param(br, int, S_IRUGO);
module_param(col, int, S_IRUGO);
module_param(flag, int, S_IRUGO);


// val must be 0 or 1
static int send_bit(int val)
{
    // set val to data pin
    gpio_set_value(23, val);

    // set 1 to clock pin
    gpio_set_value(24, 1);

    // 500ナノ秒待つ
    udelay(500);

    // set 0 to clock pin
    gpio_set_value(24, 0);

    // 500ナノ秒待つ
    udelay(500);

    return 0;
}

static int send_byte(int val)
{
    if(val < 0 || val > 255) {
        printk("val is invalid\n");
        printk("val = %d\n", val);
        val = 255;
    }
    for(int i = 7; i >= 0; i--) {
        send_bit((val >> i) & 1);
    }
    return 0;
}

static int init_gpio_number(int gpio_number) 
{
    if (!gpio_is_valid(gpio_number)){
        return 1;
    }
    gpio_request(gpio_number, "sysfs");
    gpio_direction_output(gpio_number, 0);
    return 0;
}

static void __flash(int br, int col) {
    // 明るさ
    send_byte(0xe0 | br);
    // 青
    send_byte((col & 0xff0000) / 0x10000);
    // 緑
    send_byte((col & 0x00ff00) / 0x100);
    // 赤
    send_byte(col & 0x0000ff);
}

static int flash(int br, int col, int flag)
{
    // 開始
    for(int i = 0; i < 32; i++) {
        send_bit(0);
    }

    // 各LED制御
    for (int i = 0; i < 8; i++) {

        if (!(flag & (1<<(7-i)))) {
            __flash(0, 0);
        } else {
            __flash(br, col);
        }
    }

    // 終了
    for(int i = 0; i < 32; i++) {
        send_bit(1);
    }

    return 0;
}

static int __init led_init(void)
{
    if (init_gpio_number(23)) {
        printk("gpio 23 can't init\n");
        return 0;
    }
    if (init_gpio_number(24)) {
        printk("gpio 24 can't init\n");
        return 0;
    }

    printk(KERN_INFO "driver loaded\n");
    printk(KERN_INFO "br = %d\n", br);
    printk(KERN_INFO "col = %d\n", col);
    printk(KERN_INFO "flag = %d\n", flag);

    // 各LED制御
    flash(br, col, flag);
    return 0;
}

static void __exit led_exit(void)
{
    flash(0, 0, 0);
    printk(KERN_INFO "driver unloaded\n");

    gpio_free(23);
    gpio_free(24);

    return;
}

// static int __init hello_init(void)
// {
//     printk(KERN_INFO "Hello World\n");
//     return 0;
// }
// 
// static void __exit hello_exit(void)
// {
//     printk(KERN_INFO "Bye World\n");
//     return;
// }

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Daigo Fujii");
