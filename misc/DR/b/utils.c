#include "DR.h"

/*
    __get_int_handler(int offset)

    in:     interrupt # as an offset
    out:    address of interrupt handler
*/

int __get_int_handler(int offset)
{
    int idt_entry   = 0;

                            /* off2 << 16 | off1 */
    __asm__ __volatile__ (  "xorl %%ebx,%%ebx               \n\t"
                            "pushl %%ebx                    \n\t"
                            "pushl %%ebx                    \n\t"
                            "sidt (%%esp)                   \n\t"
                            "movl 2(%%esp),%%ebx            \n\t"
                            "movl %1,%%ecx                  \n\t"
                            "leal (%%ebx, %%ecx, 8),%%esi   \n\t"
                            "xorl %%eax,%%eax               \n\t"
                            "movw 6(%%esi),%%ax             \n\t"
                            "roll $0x10,%%eax               \n\t"
                            "movw (%%esi),%%ax              \n\t"
                            "popl %%ebx                     \n\t"
                            "popl %%ebx                     \n\t"
                            : "=a" (idt_entry)
                            : "r" (offset)
                            : "ebx", "esi" );

    return idt_entry;
}

/*
    __set_int_handler(int addr, offset)

    in: function pointer to set for interrupt
    in: interrupt #
*/

void __set_int_handler(unsigned int addr, int offset)
{
                            /* off2 << 16 | off1 */
    __asm__ __volatile__ (  "xorl %%ebx,%%ebx               \n\t"
                            "pushl %%ebx                    \n\t"
                            "pushl %%ebx                    \n\t"
                            "sidt (%%esp)                   \n\t"
                            "movl 2(%%esp),%%ebx            \n\t"
                            "movl %0,%%ecx                  \n\t"
                            "leal (%%ebx, %%ecx, 8),%%edi   \n\t"
                            "movl %1,%%eax                  \n\t"
                            "movw %%ax,(%%edi)              \n\t"
                            "shrl $0x10,%%eax               \n\t"
                            "movw %%ax,6(%%edi)             \n\t"
                            "popl %%ebx                     \n\t"
                            "popl %%ebx                     \n\t"
                            "xorl %%eax,%%eax               \n\t"
                            : 
                            : "r" (offset), "r" (addr)
                            : "ebx", "edi" );

}

/*
    __get_syscall_table(int idt_entry)

    in:     Interrupt handler addr
    out:    syscall_call/syscall_table

    Return the syscall_table location based on an IDT entry addr
    or the value of syscall_call pending on mode.
*/

unsigned int __get_syscall_table(int idt_entry, int mode)
{
    unsigned char *p = (unsigned char *)idt_entry;
    unsigned int table;

    while (!((p[0] == 0xff) && (p[1] == 0x14) && (p[2] == 0x85)))
    {
        p ++;
    }

    table = *(unsigned int *)(p+3);

    /* returns sycall_table location from code */
    if (mode == RETURN_SYSCALL_TABLE)
        return table;

    /* returns syscall_call label loc to breakpoint on */
    if (mode == RETURN_SYSCALL_CALL)
        return (unsigned int)p;

    return 0;
}

/*
    __get_sysenter_call

    in:     syscall_call address
    in:     syscall_table address
    out:    sysenter_call address

    NOTE:

    Alternatively there is also a cmpl to sysenter_entry in the
    debug ENTRY .. but we want the direct offset to the syscall_table
    call in sysenter_entry anyways, so this is just as valid.

*/

unsigned int __get_sysenter_entry(unsigned int syscall_call, unsigned int table)
{
    /* do a backwards search from syscall_call for call *table(,%eax,4) */    
    unsigned char *p        = (unsigned char *)syscall_call - 1;
    unsigned int verify     = 0;

    while(!((p[0] == 0xff) && (p[1] == 0x14) && (p[2] == 0x85)))
    {
        p --;
    }

    verify = *(unsigned int *)(p+3);
    if (verify == table)
        return (unsigned int) p;

    return 0;
}

static int __syscall_table_addr;

void store_syscall_table(unsigned int addr)
{
   __syscall_table_addr = addr;
}

/*
 * return the already found one
 */
unsigned int get_syscall_table()
{
    return __syscall_table_addr;
}
