/*
 *  MHPathBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHWorldBindings.h"
#include "MHPathBindings.h"

MHPathBindings::MHPathBindings()
: RubyBindings<MHPath, true>(
    rb_define_class("MHPath", rb_cObject),
    "MHPathBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHPathBindings::Initialize), 7);
    rb_define_method(_class, "next_step", RUBY_METHOD_FUNC(MHPathBindings::NextStep), 0);
    rb_define_method(_class, "blocked?", RUBY_METHOD_FUNC(MHPathBindings::Blocked), 0);
    rb_define_method(_class, "end_of_path?", RUBY_METHOD_FUNC(MHPathBindings::EndOfPath), 0);
    rb_define_alloc_func(_class, MHPathBindings::Alloc);
}

VALUE MHPathBindings::Initialize(VALUE rSelf, VALUE rWorld, VALUE rSX, VALUE rSY, VALUE rSZ, VALUE rDX, VALUE rDY, VALUE rDZ) {
    MHPath *cSelf = Get()->getPointer(rSelf);
    MHWorld *cWorld = MHWorldBindings::Get()->getPointer(rWorld);

    cSelf->initialize(cWorld, Vector3(NUM2INT(rSX), NUM2INT(rSY), NUM2INT(rSZ)),
                              Vector3(NUM2INT(rDX), NUM2INT(rDY), NUM2INT(rDZ)));
    return rSelf;
}

VALUE MHPathBindings::NextStep(VALUE rSelf) {
    MHPath *cSelf = Get()->getPointer(rSelf);
    Vector3 cStep = cSelf->getNextStep();

    VALUE *coords = new VALUE[3];
    coords[0] = INT2NUM(cStep[0]);
    coords[1] = INT2NUM(cStep[1]);
    coords[2] = INT2NUM(cStep[2]);

    return rb_ary_new4(3, coords);
}

VALUE MHPathBindings::Blocked(VALUE rSelf) {
    MHPath *cSelf = Get()->getPointer(rSelf);
    return (cSelf->blocked()? Qtrue : Qfalse);
}

VALUE MHPathBindings::EndOfPath(VALUE rSelf) {
    MHPath *cSelf = Get()->getPointer(rSelf);
    return (cSelf->isEmpty() ? Qtrue : Qfalse);
}
