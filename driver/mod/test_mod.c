#include <linux/module.h>

static int __init lkm_init(void)
{
	printk("Hello, world.");
	int *p = (int*)0x100;
	*p = 0;
	return 0;
}

static void __exit lkm_exit(void)
{
	printk("Lkm exit...");
}

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
