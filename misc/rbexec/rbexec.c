#include <stdio.h>
#include <stdlib.h>
#include <ruby.h>

VALUE rb_bin_exec(VALUE self, VALUE str)
{
   void (*func)();

   StringValue(str);
   func = (void*) StringValuePtr(str);

   func();
}

void Init_rbexec()
{
   rb_define_global_function("bin_exec", rb_bin_exec, 1);
}
