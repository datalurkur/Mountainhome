/*
 *  MHSelectionBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHSelectionBindings.h"
#include "MHActorBindings.h"

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
    std::list <MHActor*> &selected = const_cast<std::list<MHActor*> &>(cSelf->getSelected());
    std::list <MHActor*>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(MHActorBindings::Get()->getValue(*itr));
    }
    return rSelf;
}
