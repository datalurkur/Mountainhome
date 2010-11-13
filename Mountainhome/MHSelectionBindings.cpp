/*
 *  MHSelectionBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHSelectionBindings.h"
#include "EntityBindings.h"

MHSelectionBindings::MHSelectionBindings()
: RubyBindings<MHSelection, false>(
    rb_define_class("MHSelection", rb_cObject),
    "MHSelectionBindings")
{
    rb_define_method(_class, "each", RUBY_METHOD_FUNC(MHSelectionBindings::Each), 0);
}

MHSelectionBindings::~MHSelectionBindings() {}

VALUE MHSelectionBindings::Each(VALUE rSelf) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    std::list <Entity*> &selected = cSelf->getSelected();
    std::list <Entity*>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(EntityBindings::Get()->getValue(*itr));
    }
    return rSelf;
}
