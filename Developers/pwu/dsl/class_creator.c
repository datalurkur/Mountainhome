#include "ruby.h"


// static VALUE t_init(VALUE self)
// {
//     VALUE arr;
// 
//     arr = rb_ary_new();
//     rb_iv_set(self, "@arr", arr);
//     return self;
// }
// 

static VALUE create_class(VALUE class_creator, VALUE name, VALUE super)
{
    
    return rb_define_class(StringValuePtr(name), super);
    //return rb_cObject;
}

VALUE cCC;

void Init_Class_Creator(VALUE name, VALUE is_a)
{
    cCC = rb_define_module("Class_Creator");
    rb_define_module_function(cCC, "create_class", create_class, 2);
}