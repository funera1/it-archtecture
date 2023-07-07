#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define NUM_BUFFER 1


static int module_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device Opens\n");
    return 0;
}
static int module_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device Closes\n");
    return 0;
}

static unsigned char prev_input;

static ssize_t module_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops)
{
    printk(KERN_INFO "Device Reads\n");
    if (count > NUM_BUFFER) count = NUM_BUFFER;

    // if (copy_to_user(buf, prev_input, 1) != 0) {
    //     return -EFAULT;
    // }
    printk(KERN_INFO "Input[0]: %c", prev_input);
    return count;
}
static ssize_t module_write(struct file *file, const char __user *buf, size_t count, loff_t *f_ops)
{
    printk(KERN_INFO "Device Writes\n");
    if (copy_from_user(&prev_input, buf, 1) != 0) {
        return -EFAULT;
    }
    printk(KERN_INFO "Input[0]: %c", prev_input);
    return count;
}

struct file_operations module_fops = {
    .owner = THIS_MODULE,
    .open  = module_open,
    .release = module_close,
    .read  = module_read,
    .write = module_write,
};

struct cdev *cdev;
static int __init module_initialize(void)
{
    printk(KERN_INFO "Hello World\n");

    dev_t dev = MKDEV(238, 0);
    register_chrdev_region(dev, 1, "dev_io");
    cdev = cdev_alloc();
    cdev_init(cdev, &module_fops);
    int err = cdev_add(cdev, dev, 1);
    if (err != 0) {
        printk("ERROR: cdev add\n");
    }
    return 0;
}

static void __exit module_release(void)
{
    printk(KERN_INFO "Bye World\n");
    return;
}


module_init(module_initialize);
module_exit(module_release);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Daigo Fujii");
