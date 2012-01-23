#ifndef _TPE_H
#define _TPE_H

//#ifndef CONFIG_SECURITY
//#error ">>> LSM support is not compiled in the base kernel <<<"
//#endif
#define CONFIG_SECURITY
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/security.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/list.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

#define TPE_MODULE_NAME	"TPE"
#define TPE_MODULE_VER	"0.2"

#define TPE_DEBUG(fmt, arg...)	\
		do {		\
			pr_debug("%s v%s: %s: " fmt "\n",	\
				TPE_MODULE_NAME,		\
				TPE_MODULE_VER,			\
				__FUNCTION__,			\
				##arg);				\
		} while(0)

#define TPE_INFO(fmt, arg...)	\
		do {		\
			pr_info("%s v%s: " fmt "\n",		\
				TPE_MODULE_NAME,		\
				TPE_MODULE_VER,			\
				##arg);				\
		} while(0)

#define atoi(str) simple_strtoul(((str != NULL) ? str : ""), NULL, 0)
#define TPE_MAX_TRUSTED_USERS	100
#define TPE_MAX_TRUSTED_GROUPS	100

struct tpe_trusted_file {
	char *name;
	int is_directory:1;
	struct list_head list;
};

enum tpe_subject_t {
	SUBJECT_USER,
	SUBJECT_GROUP,
	SUBJECT_FILE,
	SUBJECT_DIR
}

#endif
