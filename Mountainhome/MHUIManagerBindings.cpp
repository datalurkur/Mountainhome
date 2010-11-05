/*
 *  MHUIManagerBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHUIManagerBindings.h"
#include "MHUIElementBindings.h"
#include "MHCoreBindings.h"


MHUIManagerBindings::MHUIManagerBindings()
: RubyBindings<MHUIManager, true>(
    rb_define_class("MHUIManager", rb_cObject),
    "MHUIManagerBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHUIManagerBindings::Initialize), 2);
    rb_define_method(_class, "root=", RUBY_METHOD_FUNC(MHUIManagerBindings::SetRoot), 1);
    rb_define_method(_class, "root", RUBY_METHOD_FUNC(MHUIManagerBindings::GetRoot), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHUIManagerBindings::GetHeight), 0);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHUIManagerBindings::GetWidth), 0);
    rb_define_alloc_func(_class, MHUIManagerBindings::Alloc<MHUIManagerBindings>);
}

void MHUIManagerBindings::Mark(MHUIManager *cSelf) {
    if (cSelf->getRoot()) {
        rb_gc_mark(MHUIElementBindings::Get()->getValue(cSelf->getRoot()));
    }
}

VALUE MHUIManagerBindings::Initialize(VALUE rSelf, VALUE looknfeel, VALUE rCore) {
    std::string strLooknfeel = rb_string_value_cstr(&looknfeel);
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    MHCore *cCore = MHCoreBindings::Get()->getPointer(rCore);
    cSelf->initialize(strLooknfeel, cCore);

    return rSelf;
}

VALUE MHUIManagerBindings::SetRoot(VALUE rSelf, VALUE rElement) {
    MHUIElement *cElement = MHUIElementBindings::Get()->getPointer(rElement);
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setRoot(cElement);

    return rSelf;
}

VALUE MHUIManagerBindings::GetRoot(VALUE rSelf) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    if(cSelf->getRoot()) {
        return MHUIElementBindings::Get()->getValue(cSelf->getRoot());
    }
    else {
        Warn("UIManager has no root yet!");
        return Qnil;
    }
}

VALUE MHUIManagerBindings::GetWidth(VALUE rSelf) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHUIManagerBindings::GetHeight(VALUE rSelf) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}
