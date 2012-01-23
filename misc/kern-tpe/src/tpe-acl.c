#include "tpe.h"

#define RETURN_IF_STRICT	do {				\
					if(tpe_strict_mode)	\
						return;		\
				} while(0)

static int tpe_strict_mode;
static int tpe_trusted_mode;
static int tpe_inclusive_mode;
static int tpe_trusted_users[TPE_MAX_TRUSTED_USERS];
static int tpe_trusted_groups[TPE_MAX_TRUSTED_GROUPS];
static int tpe_trusted_users_allocated;
static int tpe_trusted_groups_allocated;

void tpe_acl_set_strict_mode(int mode)
{
	RETURN_IF_STRICT;

	tpe_strict_mode = mode;
}

void tpe_acl_set_trusted_mode(int mode)
{
	RETURN_IF_STRICT;

	tpe_trusted_mode = mode;
}

void tpe_acl_set_inclusive_mode(int mode)
{
	RETURN_IF_STRICT;

	tpe_inclusive_mode = mode;
}

void tpe_acl_generic_list_add(tpe_subject_t subject, void *data)
{
	return;
}

int tpe_acl_check(struct file *file, int safe_env)
{
	uid_t proc_uid = current->uid;
	gid_t proc_gid = current->gid;
	uid_t file_uid = file->f_dentry->d_parent->d_inode->i_uid;
	gid_t file_gid = file->f_dentry->d_parent->d_inode->i_gid;
	umode_t file_mode = file->f_dentry->d_parent->d_inode->i_mode;

	if(!(file_uid) && !(file_mode & S_IWGRP) && !(file_mode & S_IWOTH) && safe_env)
		return 0;
	
	do {
		/* Do checks */
	} while(0);


	return -EPERM;
}

int tpe_init_acl()
{
	return 0;
}

void tpe_deinit_acl()
{
	return;
}
