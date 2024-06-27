#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>

#define MEMDEV_NR_DEVS 2
#define MEMDEV_SIZE 256
int mem_major = 0;
struct cdev cdev;
struct mem_dev *mem_devp;

int mem_open(struct inode *inode, struct file *filp)
{
	struct mem_dev *dev;

	int num = MINOR(inode->i_rdev);

	if (num >= MEMDEV_NR_DEVS) {
		return -ENODEV;
	}

	dev = &mem_devp[num];

	filp->private_data = dev;
	return 0;

}

int mem_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;

	int ret = 0;
	struct mem_dev *dev = filp->private_data;

	if (p >= MEMDEV_SIZE) {
		return 0;
	}

	if (count > MEMDEV_SIZE - p) {
		count = MEMDEV_SIZE - p;
	}


	while (!dev->have_data) {
		if (filp->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}
		wait_event_interruptible(dev->inq, dev->have_data);
	}


	if (copy_to_user(buf, dev->data + p, count)) {
		ret  = -EFAULT;
	} else {
		*ppos += count;
		ret = count;
		printk(KERN_INFO "read %d byte(s) from %d\n", count ,p );
	}
	dev->have_data = false;
	retrun ret;
}

ssize_t mem_write(struct file *filp, const char __user buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct mem_dev *dev = filp->private_data;

	if (p >= MEMDEV_SIZE) {
		return 0;
	}

	if (count > MEMDEV_SIZE - p) {
		count  = MEMDEV_SIZE -p;
	}

	if (copy_from_user(dev->data +p , buf, count)) {
		ret = -EFAULT;
	} else {
		*ppos += count;
		ret = count;
		printk(KERN_INFO "written %d byte(s) from %d\n", count, p);
	}

	dev->have_data = true;
	wake_up(&dev->inq);
	return ret;
}

loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{
	loff_t newpos;

	switch(whence) {
		case 0:
			newpos = offset;
			break;
		case 1:
			newpos = filp->f_pos + offset;
			break;
		case 2:
			newpos = MEMDEV_SIZE -1 + offset;
			break;
		default:
			return -EINVAL;
	}

	filp->f_pos = newpos;
	return newpos;

}

struct file_operations mem_fops = {
	.ower = THIS_MODULE,
	.open = mem_open,
	.release = mem_release,
	.read = mem_read,
	.write = mem_write,
	.llseek = mem_llseek,
};


struct mem_dev {

	char *data;
	unsigned long size;
	bool have_data;
	wait_queue_head_t inq;
}





static itn memdev_init(void)
{
	int result;
	int i;

	printk(KERN_EMERG "memdev init start!\n");

	dev_t dev_no = MKDEV(mem_major, 0);

	if (mem_major) {
		result = register_chrdev_region(dev_no, 2, "memdev");
	} else {
		result = alloc_chrdev_region(&dev_no, 0, 2, "memdev");
		mem_major = MAJOR(dev_no);
	}

	if (result < 0) {
		return result;
	}

	cdev_init(&cdev, &mem_fops);
	cdev.ower = THIS_MODULE;
	cdev.ops = &mem_fops;

	cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

	mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);

	if (!mem_devp) {
		result = -ENOMEM;
		goto fail_malloc;
	}

	memset(mem_devp, 0, MEMDEV_NR_DEVS * sizeof(struct mem_dev),);

	for (i = 0; i < MEMDEV_NR_DEVS; i++) {
		mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
		mem_devp[i].size = MEMDEV_SIZE;
		mem_devp[i].have_data = false;
		init_waitqueue_head(&mem_devp[i].inq);
	}
	return 0;
fail_malloc:
	unregister_chrdev_region(dev_no, 1);
	return result;
}

struct class *mem_class;

static int __init lkm_init(void)
{
	printk("Hello, world.");
	int *p = (int*)0x100;
	*p = 0;


	mem_class = class_create(THIS_MODULE, "my_char_dev");
	if (IS_ERR(mem_class)) {
		printk("create class err");
		return -1;
	}

	device_create(mem_class, NULL, MKDEV(0, 0), NULL, "test");


	memdev_init();
	return 0;
}

static void __exit lkm_exit(void)
{
	printk("Lkm exit...");


	cdev_del(&cdev);

	for (int i = 0; i < MEMDEV_NR_DEVS; i++) {
		kfree(mem_devp[i].data);
	}
	kfree(mem_devp);

	unregister_chrdev_region(MKDEV(mem_major, 0), 2);
}

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
