#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x122c3a7e, "_printk" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xe2964344, "__wake_up" },
	{ 0x43d34239, "cdev_del" },
	{ 0x37a0cba, "kfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x7affd727, "kmalloc_caches" },
	{ 0x3d650a07, "kmalloc_trace" },
	{ 0xdd1b6840, "dev_set_name" },
	{ 0x8623d0e0, "device_register" },
	{ 0x14c41890, "__uio_register_device" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0x76cce6d6, "cdev_init" },
	{ 0xff0d65c6, "cdev_add" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x30bfc2c, "param_ops_int" },
	{ 0x453e7dc, "module_layout" },
};

MODULE_INFO(depends, "uio");


MODULE_INFO(srcversion, "D73473A777892D0B6F9184D");
