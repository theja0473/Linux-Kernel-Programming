/*
 * ch2/min_sysinfo/min_sysinfo.c
 ***************************************************************
 * This program is part of the source code released for the book
 *  "Linux Kernel Development Cookbook"
 *  (c) Author: Kaiwan N Billimoria
 *  Publisher:  Packt
 *  GitHub repository:
 *  https://github.com/PacktPublishing/Linux-Kernel-Development-Cookbook
 *
 * From: Ch 2: Writing your First Kernel Module-the LKM Framework
 ****************************************************************
 * Brief Description:
 * Make use of some convenience macros provided by the kernel to glean and
 * print some minimal CPU, OS details. Can be cross-compiled and tried out
 * on various Linux systems.
 *
 * For details, please refer the book, Ch 2.
 */
#include <linux/init.h>
#include <linux/module.h>

#define MYMODNAME   "min_sysinfo"
MODULE_AUTHOR("<insert your name here>");
MODULE_DESCRIPTION
    ("LKDC book:ch2/min_sysinfo: print some minimal system info");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static void lkdc_sysinfo(void)
{
	char msg[256];

	memset(msg, 0, strlen(msg));
	snprintf(msg, 47, "%s(): minimal Platform Info:\nCPU: ", __func__);

	/* Strictly speaking, all this #if... is considered ugly and should be
	   isolated as far as is possible */
#ifdef CONFIG_X86
#if(BITS_PER_LONG == 32)
	strncat(msg, "x86-32, ", 9);
#else
	strncat(msg, "x86_64, ", 9);
#endif
#endif
#ifdef CONFIG_ARM
	strncat(msg, "ARM-32, ", 9);
#endif
#ifdef CONFIG_ARM64
	strncat(msg, "Aarch64, ", 10);
#endif
#ifdef CONFIG_MIPS
	strncat(msg, "MIPS, ", 7);
#endif
#ifdef CONFIG_PPC
	strncat(msg, "PowerPC, ", 10);
#endif

#ifdef __BIG_ENDIAN
	strncat(msg, "big-endian; ", 13);
#else
	strncat(msg, "little-endian; ", 16);
#endif

#if(BITS_PER_LONG == 32)
	strncat(msg, "32-bit OS.\n", 12);
#elif(BITS_PER_LONG == 64)
	strncat(msg, "64-bit OS.\n", 12);
#endif
	pr_info("%s", msg);
}
EXPORT_SYMBOL(lkdc_sysinfo);

static int __init hello_lkdc_init(void)
{
	pr_info("%s: inserted\n", MYMODNAME);
	lkdc_sysinfo();
	return 0;	/* success */
}

static void __exit hello_lkdc_exit(void)
{
	pr_info("%s: removed\n", MYMODNAME);
}

module_init(hello_lkdc_init);
module_exit(hello_lkdc_exit);