#include "ruby.h"
#include <stdio.h>

static VALUE t_logger;

static VALUE t_init(VALUE self, VALUE logger)
{
    VALUE arr;

    arr = rb_ary_new();
    rb_iv_set(self, "@arr", arr);
    
    t_logger = logger;
    rb_iv_set(self, "@logger", arr);
    
    return self;
}


static VALUE t_add(VALUE self, VALUE anObject)
{
    VALUE arr;

    arr = rb_iv_get(self, "@arr");
    rb_ary_push(arr, anObject);
    
    printf("%s\n", RSTRING_PTR(anObject));
    
    // write to logger
    ID method = rb_intern("info");
    if (! rb_respond_to(t_logger, method))
      rb_raise(rb_eRuntimeError, "Logger should respond to 'info'!");
    
    rb_funcall(t_logger, method, 2, self, "adding");
    
    return arr;
}

// static VALUE t_multiadd(VALUE self, VALUE anString, VALUE anString2)
// {
//     rb_funcall(
// }

VALUE cTest;

void Init_Test() {
    cTest = rb_define_class("Test", rb_cObject);
    rb_define_method(cTest, "initialize", t_init, 1);
    rb_define_method(cTest, "add", t_add, 1);
    // rb_define_method(cTest, "t_multiadd", t_multiadd, 2);
}
