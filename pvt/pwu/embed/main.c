#include <stdlib.h>
#include "ruby.h"

int main(int argc, char** argv)
{
    printf("C program is calling ruby_sysinit()\n");
    ruby_sysinit(&argc, &argv);

    printf("C program is calling RUBY_INIT_STACK()\n");
    RUBY_INIT_STACK;

    printf("C program is calling ruby_init()\n");
    ruby_init();
    rb_eval_string("puts \"Hello\"");
    // rb_funcall(T_OBJECT, rb_intern("functionalfunction"), 0);

    printf("C program is calling ruby_init_loadpath()\n");
    ruby_init_loadpath();

    printf("C program is loading file: %s\n", "simple.rb");
    // void* rubyProgram = ruby_options(argc, argv);
    // ruby_run_node(rubyProgram);
    //ruby_run_node(rubyProgram);
    
    return 0;
}
