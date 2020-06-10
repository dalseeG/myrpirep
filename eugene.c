#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eugene S. Roh");
MODULE_DESCRIPTION("Linux Kernel Module writing example");

static int __init dalsee_init(void)
{
	printk(KERN_INFO "Lello World\n");
	return 0;
}

static void __exit dalsee_cleanup(void)
{
	printk(KERN_INFO "Goodbye Lello!!\n");
}

module_init(dalsee_init);
module_exit(dalsee_cleanup);
