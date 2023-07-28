#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#define NUM_BUFFER 1

static int brightness; // 明るさ
static int color; // 色
static int location; // 光らせる場所
static int locs[8];
// module_param(brightness, int, S_IRUGO);
// module_param(color, int, S_IRUGO);
// module_param(location, int, S_IRUGO);


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

static void __flash(int brightness, int color) {
    // 明るさ
    send_byte(0xe0 | brightness);
    // 青
    send_byte((color & 0xff0000) / 0x10000);
    // 緑
    send_byte((color & 0x00ff00) / 0x100);
    // 赤
    send_byte(color & 0x0000ff);
}

static int flash(int brightness, int color, int location)
{
    // 開始
    for(int i = 0; i < 32; i++) {
        send_bit(0);
    }

    // 各LED制御
    for (int i = 0; i < 8; i++) {
        if (!(location & (1<<(7-i)))) {
            __flash(0, 0);
        } else {
            __flash(brightness, color);
        }
    }

    // 終了
    for(int i = 0; i < 32; i++) {
        send_bit(1);
    }

    return 0;
}

static int led_init(void) {
    if (init_gpio_number(23)) {
        printk("gpio 23 can't init\n");
        return 0;
    }
    if (init_gpio_number(24)) {
        printk("gpio 24 can't init\n");
        return 0;
    }

    // 初期化
    brightness = 10;
    color = 100;
    location = 0;

    printk(KERN_INFO "driver loaded\n");
    printk(KERN_INFO "brightness = %d\n", brightness);
    printk(KERN_INFO "color = %d\n", color);
    printk(KERN_INFO "location = %d\n", location);

    // 各LED制御
    flash(brightness, color, location);
    return 0;
}

static void led_exit(void) {
    flash(0, 0, 0);
    printk(KERN_INFO "driver unloaded\n");

    // LED全部消す
    flash(0, 0, 255);

    gpio_free(23);
    gpio_free(24);

    return;
}

// =======================================================================
static int module_open(struct inode *inode, struct file *file)
{
    // printk(KERN_INFO "Device Opens\n");
    int minor = iminor(inode);
    file->private_data = (void *)minor;
    return 0;
}
static int module_close(struct inode *inode, struct file *file)
{
    // printk(KERN_INFO "Device Closes\n");
    return 0;
}

static unsigned char prev_input;

static ssize_t module_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops)
{
    // printk(KERN_INFO "Device Reads\n");
    if (count > NUM_BUFFER) count = NUM_BUFFER;

    int minor = (int)file->private_data;
    // locationをbufに格納する
    if (minor == 0) {
      if (copy_to_user(buf, (char*)&location, 1) != 0) {
          return -EFAULT;
      }
    } else {
      if (copy_to_user(buf, (char*)&locs[minor-1], 1) != 0) {
        return -EFAULT;
      }
    }
    return count;
}
static ssize_t module_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops)
{
    // printk(KERN_INFO "Device Writes\n");
    if (copy_from_user(&prev_input, buf, count) != 0) {
        return -EFAULT;
    }

    int minor = (int)file->private_data;
    // printk(KERN_INFO "Input[0]: %c", prev_input);

    // bufはchar型なので、その値を0-255の値に対応する、位置にledをつける
    // bufはchar型なので、int型に変換すると0-255の間の値を取る
    if (minor == 0) {
      location = (int)*buf;
      flash(brightness, color, location);
    } else {
      // minor > 0の場合bufは0 or 1のみ
      // location ^= (1<<(minor-1)) ^ buf;
      locs[minor-1] = buf;

      if (buf == 0) {
        // 消灯
        location &= ~(1 << (minor-1));
      } else {
        // 点灯
        location |= (1 << (minor-1));
      }

      flash(brightness, color, location);
    }
    return count;
}

struct file_operations module_fops = {
    .owner = THIS_MODULE,
    .open  = module_open,
    .release = module_close,
    .read  = module_read,
    .write = module_write,
};

struct cdev *cdev[9];
static int io_init(void) {
    printk(KERN_INFO "Hello World\n");

    for (int i = 0; i < 9; i++) {
      dev_t dev = MKDEV(238, i);
      register_chrdev_region(dev, 1, "dev_io");
      cdev[i] = cdev_alloc();
      cdev_init(cdev[i], &module_fops);
      int err = cdev_add(cdev[i], dev, 1);
      if (err != 0) {
          printk("ERROR: cdev add\n");
      }
    }
    return 0;
}

static void io_exit(void) {
    printk(KERN_INFO "Bye World\n");
    return;
}

static int __init module_initialize(void)
{
    led_init();
    io_init();
    return 0;
}

static void __exit module_release(void)
{
    led_exit();
    io_exit();
}


module_init(module_initialize);
module_exit(module_release);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Daigo Fujii");
