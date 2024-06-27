#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/uio_driver.h>
#include <asm/io.h>


struct mem_dev {

	char *data;
	unsigned long size;
	bool have_data;
	wait_queue_head_t inq;
};

#define MEMDEV_NR_DEVS 2
#define MEMDEV_SIZE 256

static int mem_major;

module_param(mem_major, int, S_IRUGO);
struct cdev cdev;
struct mem_dev *mem_devp;

int mem_open(struct inode *inode, struct file *filp)
{
	struct mem_dev *dev;

	dump_stack();
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

	dump_stack();
	return 0;
}

ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;

	dump_stack();
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
		printk(KERN_INFO "read %u byte(s) from %u\n", count, p );
	}
	dev->have_data = false;
	return ret;
}

ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct mem_dev *dev = filp->private_data;



	dump_stack();

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
		printk(KERN_INFO "written %u byte(s) from %u\n", count, p);
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
	.owner = THIS_MODULE,
	.open = mem_open,
	.release = mem_release,
	.read = mem_read,
	.write = mem_write,
	.llseek = mem_llseek,
};







static int memdev_init(void)
{
	int result;
	int i;

	printk(KERN_EMERG "memdev init start!\n");


	dump_stack();

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
	cdev.owner = THIS_MODULE;
	cdev.ops = &mem_fops;

	cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

	mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);

	if (!mem_devp) {
		result = -ENOMEM;
		goto fail_malloc;
	}

	memset(mem_devp, 0, MEMDEV_NR_DEVS * sizeof(struct mem_dev));

	for (i = 0; i < MEMDEV_NR_DEVS; i++) {
		mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
		mem_devp[i].size = MEMDEV_SIZE;
		mem_devp[i].have_data = false;
		init_waitqueue_head(&mem_devp[i].inq);
	}

	
	printk(KERN_EMERG "memdev init success!\n");
	return 0;
fail_malloc:
	unregister_chrdev_region(dev_no, 1);
	return result;
}

struct class *mem_class;
struct uio_info * fake_uio_info;



int fake_uio_mmap(struct uio_info *info, struct vm_area_struct *vma)
{
	printk("mmap %llx\r\n", vma->vm_start);
	dump_stack();

	
	return 0;

}


int fake_uio_open(struct uio_info *info, struct inode *inode)
{
	printk("open\r\n");
	dump_stack();
	return 0;

}

int fake_uio_release(struct uio_info *info, struct inode *inode)
{
	printk("relase\r\n");
	dump_stack();
	return 0;
}


static int __init lkm_init(void)
{
	printk("Hello, world.\b\n");

	//mem_class = class_create(THIS_MODULE, "my_char_dev");
	//if (IS_ERR(mem_class)) {
	//	printk("create class err");
	//	return -1;
	//}

	//device_create(mem_class, NULL, MKDEV(0, 0), NULL, "test");


	struct device * dummy_dev = kzalloc(sizeof(struct device), GFP_KERNEL);
	dev_set_name(dummy_dev, "fake");
	device_register(dummy_dev);



	fake_uio_info = kzalloc(sizeof(struct device), GFP_KERNEL);

	fake_uio_info->name = "fake_uio";

	fake_uio_info->version = "0.1";
	
	struct uio_mem mem = {  
			.name = "mem1",
			.addr = kzalloc(4096, GFP_KERNEL),
			.offs = 100,
			.size = 4096,
			.memtype = UIO_MEM_PHYS,
		};
	fake_uio_info->mem[0] = mem;
		
	struct uio_mem mem2 = {  
			.name = "mem2",
			.addr = kzalloc(4096 * 2, GFP_KERNEL),
			.offs = 200,
			.size = 4096 * 2,
			.memtype = UIO_MEM_LOGICAL,
		};

	fake_uio_info->mem[1] = mem2; 
	fake_uio_info->mmap = fake_uio_mmap;
	fake_uio_info->open = fake_uio_open;
	fake_uio_info->release = fake_uio_release;

	




	uio_register_device(dummy_dev, fake_uio_info);
	


	memdev_init();
	return 0;
}

static void __exit lkm_exit(void)
{
	printk("Lkm exit...\n");


	dump_stack();
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
