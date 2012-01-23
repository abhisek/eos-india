/* Abhisek Datta */
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
/* Weird */
#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1l)
#endif
#define DEBUG_FILE	"/tmp/heap-debug.txt"
#define ENV_NAME	"HEAP_DEBUG_FILE"
#undef malloc
#undef calloc
#undef realloc
#undef free

static FILE *fptr = NULL;

static void __attribute__((constructor)) open_debug_file()
{
	char *env_ptr;
	if (fptr)
		return;
	env_ptr = getenv (ENV_NAME);
	if (!env_ptr) {
	fptr = fopen (DEBUG_FILE, "a");
	if (!fptr) 
		fprintf (stderr, "Unable to open HEAP-DEBUG file: %s", strerror (errno));
	}
	else {
	fptr = fopen (env_ptr, "w");
	if (!fptr) 
		fprintf (stderr, "Unable to open HEAP-DEBUG file: %s", strerror (errno));
	}
}

static void __attribute__((destructor)) close_debug_file()
{
	if (fptr)
		fclose (fptr);
}

static void write_debug_log (const char *fmt, ...)
{
	char buffer[10000];
	va_list ap_start;

	if(!fptr)
		return;
	va_start (ap_start, fmt);
	memset (buffer,0x00,sizeof(buffer));
	vsnprintf (buffer, sizeof (buffer)-1, fmt, ap_start);
	va_end (ap_start);

	fprintf (fptr,"-----------------------------------------------\n");
	fprintf (fptr, "HEAP-DEBUG: %s\n", buffer);
}

void 
*malloc (size_t size)
{
	unsigned long (*real_malloc)(size_t);
	void *mptr;
	char *err;
	
	dlerror();
	*((void**)&real_malloc) = (void(*)(size_t))dlsym (RTLD_NEXT, "malloc");
	err = dlerror();
	if(err) {
		write_debug_log ("Cannot find original malloc address");
		return NULL;
	}
	mptr = (void*) (*real_malloc) (size);
	write_debug_log ("MALLOC: Request: %p(%d)\nChunk address: %p", size, size, (unsigned long)mptr);
	return (void*)mptr;
}

void *realloc (void *addr, size_t size)
{
	unsigned long (*real_realloc)(void*, size_t);
	void *mptr;
	char *err;

	dlerror();
	*((void**)&real_realloc) = (void(*)(void*,size_t))dlsym (RTLD_NEXT, "realloc");
	err = dlerror();
	if(err) {
		write_debug_log ("HEAP-DEBUG: Cannot find original realloc address");
		return NULL;
	}
	mptr = (void*) (*real_realloc)(addr, size);
	write_debug_log ("REALLOC: Request: %p(%d)\nReallocation address: %p\nReallocated address: %p", size, size, addr, mptr);
	return mptr;
}

void free (void *addr)
{
	unsigned long (*real_free)(void *);
	char *err;

	dlerror();
	*((void**)&real_free) = (void(*)(void*))dlsym (RTLD_NEXT, "free");
	err = dlerror();
	if(err) {
		write_debug_log ("HEAP-DEBUG: Cannot find original free address");
		return;
	}
	write_debug_log ("FREE: Chunk address: %p", (unsigned long)addr);
	(*real_free)(addr);
}

void *calloc (size_t nmemb, size_t size)
{
	unsigned long (*real_calloc)(size_t,size_t);
	void *mptr;
	char *err;
	dlerror();
	*((void**)&real_calloc) = (void(*)(size_t,size_t))dlsym (RTLD_NEXT, "calloc");
	err = dlerror();
	if(err) {
		write_debug_log ("HEAP-DEBUG: Cannot find original calloc address");
		return NULL;
	}
	mptr = (void*) (*real_calloc)(nmemb,size);
	write_debug_log ("CALLOC: Request: %p(%d)/%p(%d)\nChunk Address: %p",nmemb,nmemb,size,size,mptr);
	
	return mptr;
}
