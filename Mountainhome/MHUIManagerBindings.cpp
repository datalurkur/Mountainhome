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
#include "RenderContextBindings.h"


MHUIManagerBindings::MHUIManagerBindings()
: RubyBindings<MHUIManager, true>(
    rb_define_class("MHUIManager", rb_cObject),
    "MHUIManagerBindings")
{
    rb_define_method(_class, "root=", RUBY_METHOD_FUNC(MHUIManagerBindings::SetRoot), 1);
    rb_define_method(_class, "root", RUBY_METHOD_FUNC(MHUIManagerBindings::GetRoot), 0);
    rb_define_method(_class, "height=", RUBY_METHOD_FUNC(MHUIManagerBindings::SetHeight), 1);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHUIManagerBindings::GetHeight), 0);
    rb_define_method(_class, "width=", RUBY_METHOD_FUNC(MHUIManagerBindings::SetWidth), 1);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHUIManagerBindings::GetWidth), 0);
    rb_define_method(_class, "render", RUBY_METHOD_FUNC(MHUIManagerBindings::Render), 1);
    rb_define_alloc_func(_class, MHUIManagerBindings::Alloc<MHUIManagerBindings>);
}

void MHUIManagerBindings::Mark(MHUIManager *cSelf) {
    if (cSelf->getRoot()) {
        rb_gc_mark(MHUIElementBindings::Get()->getValue(cSelf->getRoot()));
    }
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

VALUE MHUIManagerBindings::SetWidth(VALUE rSelf, VALUE rWidth) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setWidth(NUM2INT(rWidth));
    return rSelf;
}

VALUE MHUIManagerBindings::GetWidth(VALUE rSelf) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHUIManagerBindings::SetHeight(VALUE rSelf, VALUE rHeight) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setHeight(NUM2INT(rHeight));
    return rSelf;
}

VALUE MHUIManagerBindings::GetHeight(VALUE rSelf) {
    MHUIManager *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHUIManagerBindings::Render(VALUE rSelf, VALUE rContext) {
    MHUIManager *cSelf = MHUIManagerBindings::Get()->getPointer(rSelf);
    RenderContext *cContext = RenderContextBindings::Get()->getPointer(rContext);
    cSelf->render(cContext);
    return Qnil;
}
