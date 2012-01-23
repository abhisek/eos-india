/*
 * A nice bug/backdoor like documented (man prctl) feature in the linux kernel was fixed in:
 * http://www.kernel.org/diff/diffview.cgi?file=%2Fpub%2Flinux%2Fkernel%2Fv2.6%2Fincr%2Fpatch-2.6.17.3-4.bz2;z=2
 *
 * This hotfix can be used to avoid exploitation of this kernel bug.
 *
 * This one is just a quick hack to protect few servers, tested so far only in:
 *    2.6.14-gentoo-r2
 *    Linux 2.6.16.13
 *    
 * Please use at your own risk
 *
 *          - abhisek@eos-india.net
 */
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/syscalls.h>
#include <linux/prctl.h>
#include <asm/unistd.h>

extern int loops_per_jiffy;
extern struct cpuinfo_x86 boot_cpu_data;

void **sys_call_table = NULL;

long (*orig_sys_prctl)(int, unsigned long, unsigned long,
                        unsigned long, unsigned long);

unsigned long **find_sys_call_table(void) {
   unsigned long **sctable;
   unsigned long ptr;
   unsigned long *p;

   sctable = NULL;
   for (ptr = (unsigned long)&loops_per_jiffy;ptr < (unsigned long)&boot_cpu_data; ptr += sizeof(void *)) {
      p = (unsigned long *)ptr;
      if (p[__NR_close] == (unsigned long) sys_close) {
         sctable = (unsigned long **)p;
         return &sctable[0];
      }
   }

   return NULL;
}

asmlinkage long my_sys_prctl(int option, unsigned long arg2, unsigned long arg3,
                 unsigned long arg4, unsigned long arg5)
{
   if((option == PR_SET_DUMPABLE) && (arg2 < 0 || arg2 > 1)) {
      printk(KERN_NOTICE "Denied possible exploitation attempt [uid=%d gid=%d pid=%d]\n",
            current->uid, current->gid, current->pid);
      return -EINVAL;
   }
   
   return orig_sys_prctl(option, arg2, arg3, arg4, arg5);
}

int hook_set(void)
{
   orig_sys_prctl = sys_call_table[__NR_prctl];   
   sys_call_table[__NR_prctl] = (void*) &my_sys_prctl;

   return 0;
}

int hook_unset(void)
{
   sys_call_table[__NR_prctl] = orig_sys_prctl;

   return 0;
}

int init(void)
{
   printk(KERN_NOTICE "Loading sys_prctl hotfix\n");
   sys_call_table = (void**) find_sys_call_table();
   printk(KERN_NOTICE "sys_call_table: 0x%x\n", (unsigned int)sys_call_table);
  
   if(sys_call_table)
      hook_set();
   else
      printk(KERN_NOTICE "Failed to determine sys_call_table address\n");
   
   return 0;
}

void fini(void)
{
   if(sys_call_table)
      hook_unset();
   
   printk(KERN_NOTICE "Unloading sys_prctl hotfix\n");
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("abhisek<dot>datta<at>gmail<dot>com");
MODULE_DESCRIPTION("LKM based patch for sys_prctl bug/backdoor");
