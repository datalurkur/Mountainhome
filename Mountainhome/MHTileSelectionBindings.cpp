/*
 *  MHTileSelectionBindings.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 11/25/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHTileSelectionBindings.h"

MHTileSelectionBindings::MHTileSelectionBindings()
: RubyBindings<MHTileSelection, false>(
    rb_define_class("MHTileSelection", rb_cObject),
    "MHTileSelectionBindings")
{
    rb_define_method(_class, "each", RUBY_METHOD_FUNC(MHTileSelectionBindings::Each), 0);
}

MHTileSelectionBindings::~MHTileSelectionBindings() {}

VALUE MHTileSelectionBindings::Each(VALUE rSelf) {
    MHTileSelection *cSelf = Get()->getPointer(rSelf);
    std::list <Vector3> &selected = const_cast<std::list<Vector3> &>(cSelf->getSelected());
    std::list <Vector3>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(rb_ary_new3(3, INT2NUM((*itr)[0]), INT2NUM((*itr)[1]), INT2NUM((*itr)[2])));
    }
    return rSelf;
}
