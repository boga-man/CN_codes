#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define mem_size 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manoj Boganadham");
MODULE_DESCRIPTION("A simple character device driver");

dev_t dev = 0;
uint8_t *kernel_buffer;
static struct cdev my_cdev;
static struct class *dev_class;

static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);

// all the file operations
static int device_open(struct inode *inode, struct file *file)
{
    // creating the required physical memory
    if (kernel_buffer = kmalloc(mem_size, GFP_KERNEL) == 0)
    {
        printk(KERN_INFO "Unable to allocate memory\n");
        return -1;
    }

    printk(KERN_INFO "Device has been opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk(KERN_INFO "Device file has been closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    copy_to_user(buffer, kernel_buffer, mem_size);
    printk(KERN_INFO "Device has been read\n");
    return mem_size;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
    copy_from_user(kernel_buffer, buffer, mem_size);
    printk(KERN_INFO "Device has been written\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init chr_driver_init(void)
{
    // Allocating major number
    if (alloc_chrdev_region(&dev, 0, 1, "cap_driver") < 0)
    {
        printk(KERN_INFO "Cannot allocate major number");
        return -1;
    }

    printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    // creating the cdev structure
    cdev_init(&my_cdev, &fops);

    // Add the character device to the system
    if (cdev_add(&my_cdev, dev, 1) == -1)
    {
        printk(KERN_INFO "Cannot add the device to the system");
        goto r_class;
    }

    // creating the struct class
    if (dev_class = class_create(THIS_MODULE, "my_class") == NULL)
    {
        printk(KERN_INFO "Cannot create the struct class");
        goto r_class;
    }

    // creating the device driver
    if (device_create(dev_class, NULL, dev, NULL, "my_device") == NULL)
    {
        printk(KERN_INFO "Cannot create the device driver");
        goto r_device;
    }

    // Device driver creation successfull
    printk(KERN_INFO "Device driver created successfully");

r_device:
    class_destroy(dev_class);

r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

void __exit chr_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Driver is destroyed");
}

// registering the init and exit functions
module_init(chr_driver_init);
module_exit(chr_driver_exit);
