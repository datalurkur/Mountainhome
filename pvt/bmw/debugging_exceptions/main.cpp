#include "../../../ruby/include/ruby-1.9.1/ruby.h"

static VALUE require_wrap(VALUE arg) {
    return rb_require("main.rb");
}

int main(int argc, char **argv) {
    int state;
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();

        rb_protect(require_wrap, 0, &state);
        printf("Ending state: %i\n", state);
    }

    return ruby_cleanup(state);
}
