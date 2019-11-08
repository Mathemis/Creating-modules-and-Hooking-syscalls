#include "hung_chrdev.h"


/*
 * This function is called when the module is loaded
 */
static int __init h_init(void)
{
    int ret = alloc_chrdev_region(&device_number, MINOR_FIRST, MINOR_COUNT, DEVICE_NAME);
    if (ret != 0)
    {
        printk("Hung_module: Cannot allocate a device number\n");
        return ret;
	}
    printk("Hung_module: Registered\n");
    cdev_init(&hung_cdev, &hung_fops);
    ret = cdev_add(&hung_cdev, device_number, MINOR_COUNT);
    if (ret != 0)
    {
		unregister_chrdev_region(device_number, MINOR_COUNT);
		printk("Hung_module: Cannot add device to kernel\n");
		return ret;
	}
	
	hung_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(hung_class))
	{
		cdev_del(&hung_cdev);
		unregister_chrdev_region(device_number, MINOR_COUNT);
		printk("Hung_module: Cannot create class\n");
		return PTR_ERR(hung_class);
	}
	
	hung_device = device_create(hung_class, NULL, device_number, NULL, DEVICE_NAME);
	if (IS_ERR(hung_device))
	{
        class_destroy(hung_class);
		cdev_del(&hung_cdev);
		unregister_chrdev_region(device_number, MINOR_COUNT);
		printk(KERN_WARNING "Cannot create device file\n");
		return PTR_ERR(hung_device);
	}
	memset(output_str, 0, MAX_SIZE);
	return 0;
}

/*
 * This function is called when the module is unloaded
 */
static void __exit h_cleanup(void)
{
    cdev_del(&hung_cdev);
    device_destroy(hung_class, device_number);
	class_destroy(hung_class);
	unregister_chrdev_region(device_number, MINOR_COUNT);
    printk("Hung_module: Unregistered\n");
}



module_init(h_init);
module_exit(h_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DEVICE_AUTHOR);
MODULE_DESCRIPTION(DEVICE_DESC);
