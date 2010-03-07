#include "../../ruby/include/ruby-1.9.1/ruby.h"

int main(int argc, char **argv) {
  int state;
  ruby_sysinit(&argc, &argv);
  {
    RUBY_INIT_STACK;
    ruby_init();
    rb_protect(RUBY_METHOD_FUNC(rb_require), (VALUE)"./main.rb", &state);
  }
  return ruby_cleanup(state);
}
