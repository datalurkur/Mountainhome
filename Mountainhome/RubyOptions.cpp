/*
 *  RubyOptions.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyOptions.h"

void RubyOptions::SetupBindings() {
    Class = rb_define_class("MHOptions", rb_cObject);
}
