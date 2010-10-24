/*
 *  MHSelection.cpp
 *  Mountainhome
 *
 *  Created by Paul on 9/22/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHSelection.h"

MHSelection::MHSelection() {}

void MHSelection::SetupBindings() {
    Class = rb_define_class("MHSelection", rb_cObject);
}

MHSelection::~MHSelection() {}
