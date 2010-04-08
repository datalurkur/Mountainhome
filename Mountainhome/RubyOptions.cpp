#include "RubyOptions.h"

void RubyOptions::SetupBindings() {
    Class = rb_define_class("MHOptions", rb_cObject);
}
