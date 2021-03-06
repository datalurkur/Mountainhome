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

    rb_define_method(_class, "num_actors", RUBY_METHOD_FUNC(MHSelectionBindings::NumActors), 0);
    rb_define_method(_class, "each_actor", RUBY_METHOD_FUNC(MHSelectionBindings::EachActor), 0);

    rb_define_method(_class, "num_voxels", RUBY_METHOD_FUNC(MHSelectionBindings::NumVoxels), 0);
    rb_define_method(_class, "each_voxel", RUBY_METHOD_FUNC(MHSelectionBindings::EachVoxel), 0);

    rb_define_method(_class, "remove_voxel", RUBY_METHOD_FUNC(MHSelectionBindings::RemoveVoxel), 3);
}

MHSelectionBindings::~MHSelectionBindings() {}

VALUE MHSelectionBindings::Each(VALUE rSelf) {
    EachActor(rSelf);
    EachVoxel(rSelf);

    return rSelf;
}

VALUE MHSelectionBindings::NumActors(VALUE rSelf) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->numSelectedActors());
}

VALUE MHSelectionBindings::EachActor(VALUE rSelf) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    std::list <MHActor*> &selected = const_cast<std::list<MHActor*> &>(cSelf->getSelectedActors());

    std::list <MHActor*>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(MHActorBindings::Get()->getValue(*itr));
    }

    return rSelf;
}

VALUE MHSelectionBindings::NumVoxels(VALUE rSelf) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->numSelectedVoxels());
}

VALUE MHSelectionBindings::EachVoxel(VALUE rSelf) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    std::list <Vector3> &selected = const_cast<std::list<Vector3> &>(cSelf->getSelectedVoxels());

    std::list <Vector3>::iterator itr = selected.begin();
    for(; itr != selected.end(); itr++) {
        rb_yield(rb_ary_new3(3, INT2NUM((*itr)[0]), INT2NUM((*itr)[1]), INT2NUM((*itr)[2])));
    }

    return rSelf;
}

VALUE MHSelectionBindings::RemoveVoxel(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHSelection *cSelf = Get()->getPointer(rSelf);
    cSelf->remove(Vector3(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
    return rSelf;
}
