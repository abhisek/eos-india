#ifndef __EXTERN_HOOK_TABLE__
/* external hook define file */

#define __EXTERN_HOOK_TABLE__

/* backporting Daniel's existing code to new hooking engine -bas */

/* Daniel Palacio's includes */
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/in.h>
#include <linux/dirent.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/resource.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/dcache.h>

#include <net/tcp.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/unistd.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>

#include "DR.h"

void register_hook(int, void*);
void unregister_hook(int);
void init_hook_table(void);
void uninit_hook_table(void);

/*
 * Macros for hook function's prelogue and misc. stuff
 */
#define __HOOK_PRELOGUE             void **sys_p = get_syscall_table()
#define __SYSCALL_HANDLER_ADDR(n)   sys_p[n]

#endif
