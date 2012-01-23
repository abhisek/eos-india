#define CONFIG_SECURITY
//#ifndef CONFIG_SECURITY
//#error ">>> LSM support is not enabled in kernel source <<<"
//#else

#include "kern-tpe.h"

static int tpe_debug = 1;
module_param(tpe_debug, bool, 0600);
MODULE_PARM_DESC(tpe_debug, "Enable/Disable debug flag");

static int tpe_enabled = 1;
module_param(tpe_enabled, bool, 0600);
MODULE_PARM_DESC(tpe_enabled, "Enable/Disable TPE functionality (Default: enabled)");

static int tpe_strict = 0;
module_param(tpe_strict, bool, 0600);
MODULE_PARM_DESC(tpe_strict, "Load TPE in strict mode");

static uid_t tpe_acl_uid_list[TPE_ACL_LIST_MAX_SIZE] = { [0 ... (TPE_ACL_LIST_MAX_SIZE - 1)] = -1 };
static int tpe_acl_uid_candidates = 0;
static gid_t tpe_acl_gid_list[TPE_ACL_LIST_MAX_SIZE] = { [0 ... (TPE_ACL_LIST_MAX_SIZE - 1)] = -1 };
static int tpe_acl_gid_candidates = 0;

static struct proc_dir_entry *tpe_dir;
static DEFINE_SPINLOCK(tpe_acl_lock);

static int secondary;

static void tpe_add_subject(tpe_type_t type, tpe_subject_t subject);
static void tpe_del_subject(tpe_type_t type, tpe_subject_t subject);

static int tpe_ctl_read_proc( char *page,
                              char **start,
                              off_t offset,
                              int count,
                              int *eof,
                              void *data)
{
   return -ENOSYS;
}

static int tpe_ctl_write_proc(struct file *file,
                              const char *buffer,
                              unsigned long count,
                              void *data)
{
   return -ENOSYS;
}

static int tpe_add_read_proc( char *page,
                              char **start,
                              off_t offset,
                              int count,
                              int *eof,
                              void *data)
{
   char *kbuf;
   int uid_candidates;
   int gid_candidates;
   size_t len;
   size_t i;
   int c;

   uid_candidates = tpe_acl_uid_candidates;
   gid_candidates = tpe_acl_gid_candidates;

   len = (uid_candidates * 5) + (gid_candidates * 6) + uid_candidates + gid_candidates + 1;
   kbuf = kmalloc(len + 1, GFP_KERNEL);
   if(unlikely(!kbuf))
      return -ENOMEM;

   i = 0;
   for(c = 0; (c < tpe_acl_uid_candidates) && (i < len); c++)
      i += snprintf(kbuf + i, len - i, "%d\n", tpe_acl_uid_list[c]);

   for(c = 0; (c < tpe_acl_gid_candidates) && (i < len); c++)
      i += snprintf(kbuf + i, len - i, "%%%d\n", tpe_acl_gid_list[c]);
   
   if(!i)
      return -1;

   if(offset > i)
      return -1;
   
   if(count > (i - offset))
      count = i - offset;

   memcpy(page, kbuf + offset, count);

   kfree(kbuf);
   return count;
}

static int tpe_add_write_proc(struct file *file,
                              const char *buffer,
                              unsigned long count,
                              void *data)
{
   char kbuf[16];
   char *kptr;
   size_t len;
   tpe_type_t tpe_type;
   tpe_subject_t tpe_sub;
   int retval;

   if(current->uid && current->gid)
      return -EPERM;

   len = count;
   if(len > (sizeof(kbuf) - 1))
      len = sizeof(kbuf) - 1;

   retval = copy_from_user(kbuf, buffer, len);
   if(unlikely(retval))
      return -EFAULT;

   kbuf[len] = '\0';
   if(kbuf[0] == '%') {
      tpe_type = TPE_SUBJECT_GROUP;
      kptr = &kbuf[1];
      tpe_sub.gid = atoi(kptr);
      TPE_DEBUG("Adding gid %d to trust list", tpe_sub.gid);
   }
   else {
      tpe_type = TPE_SUBJECT_USER;
      kptr = &kbuf[0];
      tpe_sub.uid = atoi(kptr);
      TPE_DEBUG("Adding uid %d to trust list", tpe_sub.uid);
   }

   tpe_add_subject(tpe_type, tpe_sub);
   
   return len;
}

static int tpe_register_procfs(void)
{
   struct proc_dir_entry *tpe_ctl;
   struct proc_dir_entry *tpe_add;
   struct proc_dir_entry *tpe_del;

   tpe_dir = proc_mkdir(TPE_MODULE_NAME, NULL);
   if(unlikely(!tpe_dir)) {
      printk(KERN_INFO "%s v%d.%d: Failed to create proc dir\n",
                        TPE_MODULE_NAME,
                        TPE_VERSION_MAJOR,
                        TPE_VERSION_MINOR);
      return -EINVAL;                        
   }

   tpe_ctl = create_proc_entry("ctl", 0600, tpe_dir);
   tpe_add = create_proc_entry("add", 0600, tpe_dir);
   tpe_del = create_proc_entry("del", 0600, tpe_dir);

   if(unlikely(!tpe_ctl) || unlikely(!tpe_add) || unlikely(!tpe_del)) {
      printk(KERN_INFO "%s v%d.%d: Failed to create proc entries\n",
                        TPE_MODULE_NAME,
                        TPE_VERSION_MAJOR,
                        TPE_VERSION_MINOR);
      return -EINVAL;                        
   }

   tpe_ctl->owner = THIS_MODULE;
   tpe_ctl->uid = 0;
   tpe_ctl->gid = 0;
   tpe_ctl->size = 0;
   tpe_ctl->read_proc = tpe_ctl_read_proc;
   tpe_ctl->write_proc = tpe_ctl_write_proc;

   tpe_add->owner = THIS_MODULE;
   tpe_add->uid = 0;
   tpe_add->gid = 0;
   tpe_add->size = 0;
   tpe_add->read_proc = tpe_add_read_proc;
   tpe_add->write_proc = tpe_add_write_proc;

   return 0;
}

static void tpe_unregister_procfs(void)
{
   remove_proc_entry("ctl", tpe_dir);
   remove_proc_entry("add", tpe_dir);
   remove_proc_entry("del", tpe_dir);

   remove_proc_entry(TPE_MODULE_NAME, NULL);
   return;
}

static void tpe_add_subject(tpe_type_t type, tpe_subject_t subject)
{
   spin_lock(&tpe_acl_lock);

   if(type == TPE_SUBJECT_USER) {
      TPE_GENERIC_LIST_INSERT(tpe_acl_uid_list, tpe_acl_uid_candidates, subject.uid);
      tpe_acl_uid_candidates++;
   } else if(type == TPE_SUBJECT_GROUP) {
      TPE_GENERIC_LIST_INSERT(tpe_acl_gid_list, tpe_acl_gid_candidates, subject.gid);
      tpe_acl_gid_candidates++;
   }
   else {
      printk(KERN_ERR "%s v%d.%d: Unknown subject type %d\n",
                        TPE_MODULE_NAME,
                        TPE_VERSION_MAJOR,
                        TPE_VERSION_MINOR,
                        type);
   }

   spin_unlock(&tpe_acl_lock);
}

static void tpe_del_subject(tpe_type_t type, tpe_subject_t subject)
{
   return;
}

static int tpe_trusted_subject(tpe_type_t type, tpe_subject_t subject)
{
   int retval;

   if(type == TPE_SUBJECT_USER)
      TPE_GENERIC_LIST_FIND(tpe_acl_uid_list, tpe_acl_uid_candidates, retval, subject.uid);
   else if(type == TPE_SUBJECT_GROUP)
      TPE_GENERIC_LIST_FIND(tpe_acl_gid_list, tpe_acl_gid_candidates, retval, subject.gid);
   else
      retval = -1;

   if(unlikely(retval >= 0))
      return 0;

   return -EPERM;
}

static int tpe_trusted_object(struct linux_binprm *bprm)
{
   struct inode *inode;
   
   inode = bprm->file->f_dentry->d_parent->d_inode;
   if(!inode->i_uid && !(inode->i_mode & S_IWGRP) && !(inode->i_mode & S_IWOTH))
      return 0;

   return -EPERM;
}

static int tpe_environment_check(struct linux_binprm *bprm)
{
   struct page *page;
   char *kaddr;
   int i, k, len;
   int retval = 0;

   for(i = 0; i < MAX_ARG_PAGES; i++) {
      page = bprm->page[i];
      if(!page)
         continue;

      kaddr = kmap(page);
      if(!kaddr)
         continue;

      k = 0;
      while(k < PAGE_SIZE) {
         len = strnlen(kaddr + k, PAGE_SIZE - k);
         if(!len) {
            k++;
         } else {
            if(!strncmp(kaddr + k, "LD_PRELOAD", 10)) {
               retval = -EPERM;
               break;
            }
            if(!strncmp(kaddr + k, "LD_AUDIT", 8)) {
               retval = -EPERM;
               break;
            }

            k += len;
         }
      }

      kunmap(page);
   }
   
   return retval;
}

static int tpe_verify(struct linux_binprm *bprm)
{
   int retval;
   tpe_subject_t sub;

   if(unlikely(!tpe_enabled) && unlikely(!tpe_strict))
      return 0;

   if((current->uid == 0) || (current->euid == 0))
      return 0;

   retval = tpe_trusted_object(bprm) || tpe_environment_check(bprm);
   if(unlikely(!retval))
      return 0;

   sub.gid = 0;
   sub.uid = current->uid;
   retval = tpe_trusted_subject(TPE_SUBJECT_USER, sub);
   if(unlikely(!retval))
      return 0;

   sub.uid = 0;
   sub.gid = current->gid;
   retval = tpe_trusted_subject(TPE_SUBJECT_GROUP, sub);
   if(unlikely(!retval))
      return 0;

   return -EPERM;
}

static int tpe_bprm_check_security(struct linux_binprm *bprm)
{
   return tpe_verify(bprm);
}

static struct security_operations tpe_security_ops = {
   .bprm_check_security = tpe_bprm_check_security
};

static int __init tpe_init(void)
{
   int retval;

   secondary = 0;
   retval = register_security(&tpe_security_ops);
   if(unlikely(retval)) {
      retval = mod_reg_security(TPE_MODULE_NAME, &tpe_security_ops);

      if(unlikely(retval)) {
         printk(KERN_INFO "Failed to register %s v%d.%d with the kernel\n",
                           TPE_MODULE_NAME,
                           TPE_VERSION_MAJOR,
                           TPE_VERSION_MINOR);
         return -EINVAL;                        
      }
      secondary = 1;
   }
   
   printk(KERN_INFO "%s v%d.%d registered with the kernel\n",
                        TPE_MODULE_NAME,
                        TPE_VERSION_MAJOR,
                        TPE_VERSION_MINOR);

   retval = tpe_register_procfs();

   return retval;
}

static void __exit tpe_exit(void)
{
   if(!secondary)
      unregister_security(&tpe_security_ops);
   else
      mod_unreg_security(TPE_MODULE_NAME, &tpe_security_ops);

   tpe_unregister_procfs();

   return;
}

security_initcall(tpe_init);
module_exit(tpe_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Kernel Security Module implementing Trusted Path Execution");
//#endif
