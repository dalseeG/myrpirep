#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define DEVICE_NAME "egchar"
#define CLASS_NAME "eg"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eugene Roh");
MODULE_DESCRIPTION("A simple Linux char driver for the raspberry Pi");

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* egcharClass = NULL;
static struct device* egcharDevice = NULL;

// The prototype functions for the character driver -- must come before the struct definition
static int	dev_open(struct inode *, struct file *);
static int	dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release
};

static int __init egchar_init(void){
	printk(KERN_INFO "RPIZW: Initializing the EGCHAR LKM\n");
	
	// ** Try to dynamically allocate amajor number for the device -- more difficult but worth it
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(majorNumber<0){
		printk(KERN_ALERT "EGCHAR failed to register a major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "EGCHAR: Registered correctly with major number %d\n", majorNumber);

	// Register the device class
	egcharClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(egcharClass)){
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(egcharClass);
	}
	printk(KERN_INFO "EGCHAR: Device class registered correctly\n");

	// Register the device driver
	egcharDevice = device_create(egcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if(IS_ERR(egcharDevice)){
		class_destroy(egcharClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(egcharDevice);
	}
	printk(KERN_INFO "EGCHAR: Device class created correctly\n");
	return 0;
}


static void __exit egchar_exit(void){
	device_destroy(egcharClass, MKDEV(majorNumber, 0));
	class_unregister(egcharClass);
	class_destroy(egcharClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "EGCHAR: Goodbye from the LKM!!!!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "EGCHAR: Device has been opend %d time(s)\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	error_count = copy_to_user(buffer, message, size_of_message);

	if(error_count == 0){
		printk(KERN_INFO "EGCHAR: Sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);
	}
	else{
		printk(KERN_INFO "EGCHAR: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	sprintf(message, "%s(%zu letters)", buffer, len);
	size_of_message = strlen(message);
	printk(KERN_INFO "EGCHAR: Received %zu characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "EGCHAR: Device successfully closed\n");
	return 0;
}

module_init(egchar_init);
module_exit(egchar_exit);
	
