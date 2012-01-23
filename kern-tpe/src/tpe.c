#include "tpe.h"

static int secondary;

static int tpe_bprm_check_env(struct linux_binprm *bprm)
{
	struct page *page;
	char *kaddr, **kptr;
	int i, k, len;
	int retval = 0;
	static char *unsafe_env_vars[] = {
		"LD_PRELOAD",
		"LD_AUDIT",
		"LD_DEBUG",
		"LD_LIBRARY_PATH",
		"_RTLD_",
		NULL
	};

	for(i = 0; (i < MAX_ARG_PAGES) && (retval == 0); i++) {
		page = bprm->page[i];
		if(!page)
			continue;

		kaddr = kmap(page);
		if(!kaddr)
			continue;

		k = 0;
		while((k < PAGE_SIZE) && (retval == 0)) {
			len = strnlen(kaddr + k, PAGE_SIZE - k);
			if(unlikely(!len)) {
				k++;
			}
			else {
				kptr = unsafe_env_vars[0];
				for(kptr = unsafe_env_vars; *kptr != NULL; kptr += sizeof(char*)) {
					if(!strncmp(kaddr + k, *kptr, min(strlen(*kptr), len))) {
						retval = -EPERM;
						break;
					}
				}
				k += len;
			}
		}
		kunmap(page);
	}

	return retval;
}

static int tpe_bprm_check_security(struct linux_binprm *bprm)
{
	int retval;

	if(unlikely(current->uid == 0))
		return 0;
	
	retval = tpe_acl_check(bprm->file, !tpe_bprm_check_env(bprm));

	return retval;
}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 20))
static int tpe_file_mmap(struct file *file, unsigned long reqprot,
			unsigned long prot,
			unsigned long flags)
#else
static int tpe_file_mmap(struct file *file, unsigned long reqprot,
			unsigned long prot,
			unsigned long flags,
			unsigned long addr,
			unsigned long addr_only)
#endif
{
	int retval;
	char *fptmp, *exepath;

	if((unlikely(current->uid == 0)) || (unlikely(reqprot != PROT_EXEC)) || (unlikely(file == NULL)))
		return 0;
	
	retval = tpe_acl_check(file, 1);
	if(retval) {
		fptmp = (char*)__get_free_page(GFP_KERNEL);
		if(unlikely(fptmp == NULL))
			return -ENOMEM;

		exepath = d_path(file->f_dentry, file->f_vfsmnt, fptmp, PAGE_SIZE);
		TPE_INFO("Denied mmap of %s by uid: %d gid: %d pid: %d", exepath, current->uid, current->gid, current->pid);

		free_page((unsigned long)fptmp);
	}

	return retval;
}

static int tpe_file_mprotect(struct vm_area_struct *vma,
				unsigned long reqprot,
				unsigned long prot)
{
	int retval;
	char *fptmp, *exepath;

	if((unlikely(current->uid == 0)) || (unlikely(reqprot != PROT_EXEC)) || (unlikely(vma->vm_file == NULL)))
		return 0;
	
	retval = tpe_acl_check(vma->vm_file, 1);
	if(retval) {
		fptmp = (char*)__get_free_page(GFP_KERNEL);
		if(unlikely(fptmp == NULL))
			return -ENOMEM;

		exepath = d_path(vma->vm_file->f_dentry, vma->vm_file->f_vfsmnt, fptmp, PAGE_SIZE);
		TPE_INFO("Denied mprotect on %s by uid: %d gid: %d pid: %d", exepath, current->uid, current->gid, current->pid);

		free_page((unsigned long)fptmp);
	}

	return retval;
}

static struct security_operations tpe_security_ops = {
	.bprm_check_security = tpe_bprm_check_security,
	.file_mmap = tpe_file_mmap,
	.file_mprotect = tpe_file_mprotect
};

int tpe_init(void)
{
	int retval;

	secondary = 0;
	retval = register_security(&tpe_security_ops);
	if(unlikely(retval)) {
		retval = mod_reg_security(TPE_MODULE_NAME, &tpe_security_ops);
		if(unlikely(retval)) {
			TPE_INFO("Failed to register with kernel");

			return -EINVAL;
		}
		secondary = 1;
	}

	retval = tpe_register_procfs();
	if(unlikely(retval)) {
		tpe_exit();
		return -EINVAL;
	}

	retval = tpe_init_acl();
	if(unlikely(retval)) {
		tpe_exit();
		return -EINVAL;
	}

	retval = tpe_init_conf();
	if(unlikely(retval)) {
		tpe_exit();
		return -EINVAL;
	}
	
	TPE_INFO("Registered successfully with the kernel");
	return 0;
}

void tpe_exit(void)
{
	if(!secondary)
		unregister_security(&tpe_security_ops);
	else
		mod_unreg_security(TPE_MODULE_NAME, &tpe_security_ops);
	
	tpe_unregister_procfs();
	tpe_deinit_conf();
	TPE_INFO("Unregistered from the kernel");
}

security_initcall(tpe_init);
module_exit(tpe_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Kernel Security Module implementing Trusted Path Execution");
