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
	{ 0x122c3a7e, "_printk" },
	{ 0xd196c272, "cdev_add" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0xc699825e, "cdev_init" },
	{ 0x3115262c, "cdev_alloc" },
	{ 0x1719cc94, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "8608D3A5E76012DB04E644F");
