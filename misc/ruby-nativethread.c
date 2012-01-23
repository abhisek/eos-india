/*
 * FAILED attempt :(
 */
#include <ruby.h>
#if defined(WIN32) && defined(_WIN32)
   #include <windows.h>
#else
   #include <pthread.h>
   #include <errno.h>
   #include <signal.h>
#endif

/*
 * Debugging macro
 *
 *    - stdio library might not be thread safe?
 */
#define DEBUG_PRINT(x, ...)                                                   \
         CRITICAL_SECTION_BEGIN;                                              \
        ((RTEST(ruby_debug) && RTEST(ruby_verbose))?                          \
        (fprintf(stderr, "nativethread: "x"\n", ##__VA_ARGS__),fflush(stderr)) : 0); \
         CRITICAL_SECTION_END;

/*
 * class VALUE
 */
VALUE rb_nThreadObject;
VALUE rb_nThreadError;

/*
 * Definition of parameters required by the thread_starter_routine
 * to yield ruby code (in safe way)
 */
typedef struct {
   VALUE th_codeblock;
   VALUE th_args;
} rb_nativethread_ex_starter_t;

/*
 * OS dependent native thread handle
 */
#if defined(WIN32) || defined(_WIN32)
typedef HANDLE    rb_nativethread_ex_t;
#else
typedef pthread_t rb_nativethread_ex_t;
#endif

/*
 * main thread
 */
static rb_nativethread_ex_t main_thread;
static VALUE main_thread_obj;

/*
 * Defines the list of created/active threads
 */
typedef struct rb_nativethread_ex_list {
   VALUE                            th_obj;
   struct rb_nativethread_ex_list   *th_next;
} rb_nativethread_ex_list_t;

static rb_nativethread_ex_list_t *thread_list = NULL;
#define LIST_ADD_NATIVETHREAD(obj)  do {  \
                                       CRITICAL_SECTION_BEGIN;          \
                                       rb_nativethread_ex_list_t *_item = ALLOC(rb_nativethread_ex_list_t); \
                                       _item->th_obj = obj;             \
                                       _item->th_next = thread_list;    \
                                       thread_list = _item;             \
                                       CRITICAL_SECTION_END;            \
                                    } while(0)
#define FOR_EACH_NATIVETHREAD       for(rb_nativethread_ex_list_t *i = thread_list; i; i = i->next)

/*
 * Synchronization for handling global thread list
 */
#if defined(WIN32) || defined(_WIN32)
   static CRITICAL_SECTION csObject;
   #define CRITICAL_SECTION_BEGIN   EnterCriticalSection(&csObject)
   #define CRITICAL_SECTION_END     LeaveCriticalSection(&csObject)
#else
   static pthread_mutex_t rb_fastmutex = PTHREAD_MUTEX_INITIALIZER;
   #define CRITICAL_SECTION_BEGIN   pthread_mutex_lock(&rb_fastmutex)
   #define CRITICAL_SECTION_END     pthread_mutex_unlock(&rb_fastmutex)
#endif

/*
 * Thread initializer
 */

static VALUE rb_nativethread_ex_init(VALUE argc, VALUE *argv, VALUE self)
{
   DEBUG_PRINT("initializing nativethread class");

   return self;
}

/*
 * Internal thread_starter_routine
 */

static void rb_nativethread_ex_starter(void *param)
{
   DEBUG_PRINT("yielding ruby");
   /*
    * Lets go ruby
    */
   rb_yield((VALUE)param);
}

/*
 * call-seq:
 *    Thread.new([arg]*) {|args| block }   => thread
 *    Thread.start([arg]*) {|args| block }   => thread
 *    Thread.fork([arg]*) {|args| block }   => thread
 *    
 * Creates a new native thread using (depending on platform)
 * a) pthread_create (unix)
 * b) CreateThread (win32)
 *
 * The internal thread_starter_routine is called as a new thread which initializes
 * ruby code in its current context yielding the codeblock
 *
 * The thread creation must be handled with care since ruby maintains codeblocks
 * in its eval tree and must be considered running in critical section unless
 * the code is executing
 */

static VALUE rb_nativethread_ex_start(VALUE self, VALUE args)
{
   rb_nativethread_ex_t          th;
   int                           ret;
   char                          *msgBuf;
   VALUE                         obj;
   VALUE                         codeblock;

   if(!rb_block_given_p()) {
      rb_raise(rb_nThreadError, "must be called with a block");
   }
   codeblock = rb_block_proc();

   obj = rb_obj_alloc(rb_nThreadObject);
   rb_obj_call_init(obj, 0, NULL);
  
   DEBUG_PRINT("Initializing native thread");

#if defined(WIN32) || defined(_WIN32)
   th = CreateThread(NULL, 
                  0, 
                  (LPTHREAD_START_ROUTINE)&rb_nativethread_ex_starter, 
                  (LPVOID)self, 
                  0, 
                  NULL);
   if(th == INVALID_HANDLE) {
      FormatMessage(
               FORMAT_MESSAGE_ALLOCATE_BUFFER |
               FORMAT_MESSAGE_FROM_SYSTEM |
               FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL,
               GetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               &msgBuf
            );
      rb_raise(rb_eRuntimeError, "%s", msgBuf);
   }
#else
   ret = pthread_create(&th, 
                  NULL, 
                  (void*)&rb_nativethread_ex_starter, 
                  (void*)self);
   if(ret) {
      rb_raise(rb_eRuntimeError, "%s", strerror(errno));
   }
#endif

   sleep(1);   /* Let the thread initialize - TODO: Fix possible race condition */
   DEBUG_PRINT("native thread initialized (tid: %d)", th);
   
   rb_iv_set(obj, "@handle", INT2FIX(th));

   return obj;
}

/*
 * call-seq:
 *    NativeThread#join
 *
 * waits for the thread object to finish
 */

static VALUE rb_nativethread_ex_join(VALUE self)
{
#if defined(WIN32) && defined(_WIN32)
#else
   VALUE v = rb_iv_get(self, "@handle");
   
   DEBUG_PRINT("joining thread (%d)", NUM2INT(v));
   
   pthread_join(NUM2INT(v), NULL);
#endif

   return Qnil;
}

/*
 * call-seq:
 *    NativeThread#exit
 *
 * exits the calling thread
 */
static VALUE rb_nativethread_ex_exit(VALUE self)
{
#if defined(WIN32) && defined(_WIN32)
   ExitThread(0);
#else
   pthread_exit(NULL);
#endif
}

/*
 * call-seq:
 *    NativeThread#current
 *
 *    returns the calling thread's handle
 */
static VALUE rb_nativethread_ex_current(VALUE self)
{
   VALUE obj;
   
   obj = rb_obj_alloc(rb_nThreadObject);
   rb_obj_call_init(obj, 0, NULL);
   rb_iv_set(obj, "@handle", INT2FIX(pthread_self()));
   
   return obj;
}

/*
 * call-seq:
 *    NativeThread#kill
 *
 *    kills the thread specified by the thread handle th
 */
static VALUE rb_nativethread_ex_kill(VALUE self, VALUE th)
{
   pthread_kill(NUM2INT(rb_iv_get(th,"@handle")), SIGKILL);
}

void Init_nativethread()
{
   VALUE obj;
   /*
    * Initialize critical section (win32)
    */
#if defined(WIN32) || defined(_WIN32)
   InitializeCriticalSection(&csObject);
   main_thread = GetCurrentThread();
#else
   main_thread = pthread_self();
#endif

   rb_nThreadError = rb_define_class("NativeThreadError", rb_eStandardError);
   rb_nThreadObject = rb_define_class("NativeThread", rb_cObject);

   /*
    * Create the object of the main thread and store 
    * it in global list
    */
   obj = rb_obj_alloc(rb_nThreadObject);
   rb_obj_call_init(obj, 0, NULL);
   rb_iv_set(obj, "@handle", INT2FIX(main_thread));
   main_thread_obj = obj;
   LIST_ADD_NATIVETHREAD(obj);   

   /*
    * Singleton method definitions
    */
   rb_define_singleton_method(rb_nThreadObject, "new", rb_nativethread_ex_start, -2);
   rb_define_singleton_method(rb_nThreadObject, "start", rb_nativethread_ex_start, -2);
   rb_define_singleton_method(rb_nThreadObject, "fork", rb_nativethread_ex_start, -2);
   rb_define_singleton_method(rb_nThreadObject, "exit", rb_nativethread_ex_exit, 0);
   rb_define_singleton_method(rb_nThreadObject, "current", rb_nativethread_ex_current, 0);
   rb_define_singleton_method(rb_nThreadObject, "kill", rb_nativethread_ex_kill, 1);

   /*
    * Instance methods
    */
   rb_define_method(rb_nThreadObject, "initialize", rb_nativethread_ex_init, -2);
   rb_define_method(rb_nThreadObject, "join", rb_nativethread_ex_join, 0);
}
