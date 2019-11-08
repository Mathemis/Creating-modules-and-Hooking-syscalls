#ifndef HUNG_CHRDEV_H
#define HUNG_CHRDEV_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/random.h>


#define DEVICE_AUTHOR "Cao Nhon Hung - 1712475"
#define DEVICE_NAME "hung_chrdev"
#define DEVICE_DESC "Kernel module give character device: user space can open and read random number"
#define MINOR_FIRST 0
#define MINOR_COUNT 1
#define MAX_SIZE 33
#define MAX_RAND 1000


static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

/* 
 * Global variables are declared as static, so are global within the file. 
 */
static struct cdev hung_cdev;
static struct class *hung_class;
static struct device *hung_device;
static int Device_Open = 0;	/* Is device open? Used to prevent multiple access to device */

static struct file_operations hung_fops = {
    .owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static dev_t device_number; 

static char output_str[MAX_SIZE];




/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *i, struct file *file)
{
	if (Device_Open)
		return -EBUSY;
	Device_Open++;
	try_module_get(THIS_MODULE);
    printk("Hung_chrdev: open()\n");
    return 0;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		// We're now ready for our next caller 
	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);
    printk("Hung_chrdev: close()\n");
	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, char __user *buf, size_t len, loff_t * offset)
{
	printk("Hung_chrdev: read a random number from chardev\n");
	static int randNum;
	get_random_bytes(&randNum, sizeof(int));
	randNum %= MAX_RAND;
        while(randNum && randNum % 10 == 0)
	        randNum /= 10;

    ssize_t index = 0;
	if (randNum < 0){
		index += sprintf(output_str + index, "-");
		randNum *= -1;
	}
	while (randNum)
	{
		index += sprintf(output_str + index, "%d", randNum % 10);
		randNum /= 10;
	}
    output_str[index] = '\0';

	ssize_t output_str_size = strlen(output_str);
    /* function to copy kernel space buffer to user space*/
	if (copy_to_user(buf, output_str, output_str_size) != 0)
	{
        printk("KernelSpace->UserSpace copy failed!\n");
        return 0;
    }   
    return output_str_size;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk("Driver: write()\n");
	return -EINVAL;
}


#endif /* HUNG_CHRDEV_H */
