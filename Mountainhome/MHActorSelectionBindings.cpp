/*
 *  MHActorSelectionBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHActorSelectionBindings.h"
#include "MHActorBindings.h"

MHActorSelectionBindings::MHActorSelectionBindings()
: RubyBindings<MHActorSelection, false>(
    rb_define_class("MHActorSelection", rb_cObject),
    "MHActorSelectionBindings")
{
    rb_define_method(_class, "each", RUBY_METHOD_FUNC(MHActorSelectionBindings::Each), 0);
}

MHActorSelectionBindings::~MHActorSelectionBindings() {}

VALUE MHActorSelectionBindings::Each(VALUE rSelf) {
    MHActorSelection *cSelf = Get()->getPointer(rSelf);
    std::list <MHActor*> &selected = const_cast<std::list<MHActor*> &>(cSelf->getSelected());
    std::list <MHActor*>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(MHActorBindings::Get()->getValue(*itr));
    }
    return rSelf;
}
