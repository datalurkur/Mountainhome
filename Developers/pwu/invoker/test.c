#include <stdlib.h>
#include "ruby.h"

int main(int argc, char **argv) {
    ruby_sysinit(&argc, &argv);
    {
        RUBY_INIT_STACK;
        ruby_init();
        return ruby_run_node(ruby_options(argc, argv));
    }
    while(1) { printf("fewew");}
}
