/*
 *  MHSelectionBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHSelectionBindings.h"

MHSelectionBindings::MHSelectionBindings()
: RubyBindings<MHSelection, false>(
    rb_define_class("MHSelection", rb_cObject),
    "MHSelectionBindings")
{}

MHSelectionBindings::~MHSelectionBindings() {}

