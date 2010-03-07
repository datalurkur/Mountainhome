#include "ruby.h"


static VALUE t_init(VALUE self)
{
    VALUE arr;

    arr = rb_ary_new();
    rb_iv_set(self, "@arr", arr);
    return self;
}


static VALUE t_add(VALUE self, VALUE anObject)
{
    VALUE arr;

    arr = rb_iv_get(self, "@arr");
    rb_ary_push(arr, anObject);
    return arr;
}

VALUE cTest;

void Init_Test() {
    cTest = rb_define_class("Test", rb_cObject);
    rb_define_method(cTest, "initialize", t_init, 0);
    rb_define_method(cTest, "add", t_add, 1);
}