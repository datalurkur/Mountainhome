#include <stdlib.h>
#include "ruby.h"

VALUE counter;

void define_globals() {
    rb_define_variable("$counter", &counter);
    counter = INT2NUM(0);
}

int main(int argc, char** argv)
{
        printf("C program is calling ruby_sysinit()\n");
        ruby_sysinit(&argc, &argv);

        printf("C program is calling RUBY_INIT_STACK()\n");
        RUBY_INIT_STACK;

        printf("C program is calling ruby_init()\n");
        ruby_init();
        ruby_init_loadpath();

        printf("C is defining globals()\n");
        define_globals();

        rb_eval_string("puts \"Ruby-in-C: #{$counter}\"");

        counter = rb_gv_get("$counter");
        printf("C: %i\n", NUM2INT(counter));

        int tmp = NUM2INT(counter);
        while(tmp < 50) {
            tmp = NUM2INT(counter);
            ++tmp;
            printf("C: tmp: %i\n", tmp);
            counter = INT2NUM(tmp);
            printf("C: %i\n", NUM2INT(counter));
            rb_gv_set("$counter", counter);
            rb_eval_string("puts \"Ruby-in-C: #{$counter}\"");
        }
        printf("C is requiring simple.rb\n");
        rb_require("simple");
    
    return ruby_cleanup(0);;
}
