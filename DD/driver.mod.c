#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
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
	{ 0xe9a850a9, "module_layout" },
	{ 0x8bd18bae, "cdev_del" },
	{ 0xc47a0a94, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x3a0bca50, "class_destroy" },
	{ 0xfd048de0, "device_create" },
	{ 0xbffcce80, "__class_create" },
	{ 0x82500450, "cdev_add" },
	{ 0x6cdf1bb2, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xdbee476f, "kmem_cache_alloc_trace" },
	{ 0xbd0c4c0c, "kmalloc_caches" },
	{ 0xc5850110, "printk" },
	{ 0x37a0cba, "kfree" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "73775B0A5CE434644C4A71B");
