#include <ruby.h>

static VALUE require_wrap(VALUE arg) {
    return rb_require("main.rb");
}

int main(int argc, char **argv) {
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();
        ruby_script("testing!!!!");

        rb_gv_set("$brent", ID2SYM(rb_intern("c_defined_symbol")));

        int state;
        rb_protect(require_wrap, 0, &state);
    }

    return ruby_cleanup(0);
}
