#include "tpe.h"

static const char *tpe_proc_dir_name = "tpe";
static const char *tpe_proc_ctl_name = "ctl";
static struct proc_dir_entry *tpe_proc_dir;
static struct proc_dir_entry *tpe_proc_entry;

static int tpe_proc_read(char *page,
			char **start,
			off_t offset,
			int count,
			int *eof,
			void *data)
{
	return -ENOSYS;
}

static int tpe_proc_write(struct file *file,
			const char *buffer,
			unsigned long count,
			void *data)
{
	return -ENOSYS;
}

static int tpe_init_procfs()
{
	tpe_proc_dir = proc_mkdir(tpe_proc_dir_name, NULL);
	if(unlikely(!tpe_proc_dir)) {
		TPE_INFO("Failed to create proc interface");
		return -EINVAL;
	}
	
	tpe_proc_entry = create_proc_entry(tpe_proc_ctl_name, 0600, tpe_proc_dir);
	if(unlikely(!tpe_proc_entry)) {
		TPE_INFO("Failed to register proc interface");
		return -EINVAL;
	}

	tpe_proc_entry->owner = THIS_MODULE;
	tpe_proc_entry->uid = tpe_proc_entry->gid = 0;
	tpe_proc_entry->size = 0;
	tpe_proc_entry->read_proc = tpe_proc_read;
	tpe_proc_entry->write_proc = tpe_proc_write;

	return 0;
}

static void tpe_deinit_procfs()
{
	if(unlikely(tpe_proc_entry)) {
		remove_proc_entry(tpe_proc_ctl_name, tpe_proc_dir);
	}
	if(unlikely(tpe_proc_dir)) {
		remove_proc_entry(tpe_proc_dir_name, NULL);
	}
}

int tpe_register_procfs()
{
	return tpe_init_procfs();	
}

void tpe_unregister_procfs()
{
	tpe_deinit_procfs();
}
