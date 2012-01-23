/*
 * Shared Data Structure implementation
 */

#include <main.h>

char *memfile     = "pmem.dat";
char *access_file = "req.dat";

int P;
int M;
int N;
int T;
int F;
int R;

int *page_table;
int **vm;

struct memory_frame *memory;

struct pf_request_head pf_requests;
struct pf_response_head pf_responses;

pthread_mutex_t ml = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t vml = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t pfreql = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pfresl = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t pgtl = PTHREAD_MUTEX_INITIALIZER;

pthread_t pam_thread;
pthread_t pfh_thread;
pthread_t arh_thread;

void lock_memory()
{
   pthread_mutex_lock(&ml);
}

void unlock_memory()
{
   pthread_mutex_unlock(&ml);
}

void lock_vm()
{
   pthread_mutex_lock(&vml);
}

void unlock_vm()
{
   pthread_mutex_unlock(&vml);
}

void lock_pagetable()
{
   pthread_mutex_lock(&pgtl);
}

void unlock_pagetable()
{
   pthread_mutex_unlock(&pgtl);
}

void pf_request_queue_lock()
{
   pthread_mutex_lock(&pfreql);
}

void pf_request_queue_unlock()
{
   pthread_mutex_unlock(&pfreql);
}

void pf_response_queue_lock()
{
   pthread_mutex_lock(&pfresl);
}

void pf_response_queue_unlock()
{
   pthread_mutex_unlock(&pfresl);
}

void verify_memory_access(int frame, int word)
{
   if((frame >= N) || (word >= P)) {
      UI_MSG("Invalid Memory Access");
      exit(EXIT_FAILURE);
   }
}

void verify_vm_access(int page, int word)
{
   if((page >= M) || (word >= P)) {
      UI_MSG("Invalid Virtual Memory Access");
      exit(EXIT_FAILURE);
   }
}

