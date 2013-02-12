#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4a3dbd89, "module_layout" },
	{ 0x784cfac3, "cdev_alloc" },
	{ 0xe3cc56ec, "kmalloc_caches" },
	{ 0x389150a0, "cdev_init" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xc4516c59, "nonseekable_open" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0x65d6d0f0, "gpio_direction_input" },
	{ 0x27e1a049, "printk" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x504e95e7, "cdev_add" },
	{ 0x11f447ce, "__gpio_to_irq" },
	{ 0xc2165d85, "__arm_iounmap" },
	{ 0x8dd2508a, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5E112CBB2F7CE6E7E9B09BE");
