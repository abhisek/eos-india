#ifndef _TPE_H
#define _TPE_H

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
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

#define TPE_MODULE_NAME "tpe"
#define TPE_VERSION_MAJOR 0
#define TPE_VERSION_MINOR 1

#define TPE_DEBUG(fmt, arg...)      \
         do {                       \
            if(tpe_debug)           \
               printk(KERN_DEBUG "%s: %s: " fmt "\n",   \
               TPE_MODULE_NAME, __FUNCTION__,      \
               ## arg);                            \
         } while(0)

#define  TPE_MSG(fmt, arg...)       \
         do {  \
            printk(KERN_INFO "%s v%d.%d: " fmt "\n",  \
               TPE_MODULE_NAME,  \
               TPE_VERSION_MAJOR,   \
               TPE_VERSION_MINOR,   \
               ## arg); \
         } while(0)

#define TPE_ACL_LIST_MAX_SIZE 100

typedef enum {
   TPE_SUBJECT_USER,
   TPE_SUBJECT_GROUP
} tpe_type_t;

typedef union {
   uid_t uid;
   gid_t gid;
} tpe_subject_t;

#define atoi(str) simple_strtoul(((str != NULL) ? str : ""), NULL, 0)

#define TPE_GENERIC_LIST_INSERT(list, lsize, val) \
   do {  \
      int _pi; \
      if(lsize >= TPE_ACL_LIST_MAX_SIZE) { \
         printk(KERN_ERR "%s v%d.%d: tpe acl list overflow detected\n",   \
                           TPE_MODULE_NAME,  \
                           TPE_VERSION_MAJOR,   \
                           TPE_VERSION_MINOR);  \
      }  \
      else { \
         _pi = lsize - 1; \
         while(_pi >= 0 && list[_pi] > val) { \
            list[_pi + 1] = list[_pi]; \
            _pi--; \
         } \
         list[_pi + 1] = val; \
      } \
   } while(0)

#define TPE_GENERIC_LIST_DELETE(list, lsize, val) \
   do { \
      int _pi; \
      if(lsize <= 0) {  \
         printk(KERN_ERR "%s v%d.%d: tpe acl list underflow detected\n",   \
                           TPE_MODULE_NAME,  \
                           TPE_VERSION_MAJOR,   \
                           TPE_VERSION_MINOR);  \
      }  \
      else {   \
         for(_pi = 0; (_pi < lsize) && (list[_pi] != val); _pi++) \
            ; \
         if(_pi < lsize) { \
            for(;_pi < (lsize - 1); _pi++) \
               list[_pi] = list[_pi + 1]; \
         } \
      }  \
   } while(0)

#define TPE_GENERIC_LIST_FIND(list, lsize, index, val) \
   do {  \
      int _low;   \
      int _high;  \
      int _mid;   \
      \
      _low = 0;   \
      _high = lsize; \
      _mid = (_low + _high) / 2; \
      index = -1; \
      \
      while((_low < _high) && (list[_mid] != val)) {  \
         if(val > list[_mid]) {  \
            _low = _mid; \
            _mid = (_low + _high) / 2; \
         }  \
         else {   \
            _high = _mid;  \
            _mid = (_low + _high) / 2; \
         }  \
      }  \
      if(list[_mid] == val)   \
         index = _mid;  \
   } while(0)

#endif
