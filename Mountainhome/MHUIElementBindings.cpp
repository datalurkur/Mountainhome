/*
 *  MHUIElementBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHUIManagerBindings.h"
#include "MHUIElementBindings.h"

// MHUIElements are Entities, so inherit Entity's Ruby bindings.
MHUIElementBindings::MHUIElementBindings()
: RubyBindings<MHUIElement, true>(
    rb_define_class("MHUIElement", EntityBindings::Get()->getClass()),
    "MHUIElementBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHUIElementBindings::Initialize), 0);

    rb_define_method(_class, "add_child", RUBY_METHOD_FUNC(MHUIElementBindings::AddChild), 1);
    rb_define_method(_class, "delete_child", RUBY_METHOD_FUNC(MHUIElementBindings::DeleteChild), 1);
    rb_define_method(_class, "delete_children", RUBY_METHOD_FUNC(MHUIElementBindings::DeleteChildren), 0);
    rb_define_method(_class, "each_child", RUBY_METHOD_FUNC(MHUIElementBindings::EachChild), 0);

    rb_define_method(_class, "always_on_top", RUBY_METHOD_FUNC(MHUIElementBindings::AlwaysOnTop), 0);

    rb_define_alloc_func(_class, MHUIElementBindings::Alloc<MHUIElementBindings>);
}

void MHUIElementBindings::Mark(MHUIElement *cSelf) {
    std::list<MHUIElement*> &children = const_cast<std::list<MHUIElement*> &>(cSelf->getChildren());

    std::list<MHUIElement*>::iterator itr = children.begin();
    for (; itr != children.end(); itr++) {
        rb_gc_mark(MHUIElementBindings::Get()->getValue(*itr));
    }
}

VALUE MHUIElementBindings::Initialize(VALUE rSelf) {
    MHUIElement *cObj = new MHUIElement();
    return NEW_RUBY_OBJECT(MHUIElementBindings, cObj);
}

#pragma mark Child Functions

VALUE MHUIElementBindings::AddChild(VALUE rSelf, VALUE rChild) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cChild = Get()->getPointer(rChild);
    cSelf->addChild(cChild);
    return rSelf;
}

VALUE MHUIElementBindings::DeleteChild(VALUE rSelf, VALUE rChild) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cChild = Get()->getPointer(rChild);
    cSelf->deleteChild(cChild);
    return rSelf;
}

VALUE MHUIElementBindings::DeleteChildren(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->deleteAllChildren();
    return rSelf;
}

VALUE MHUIElementBindings::EachChild(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    std::list<MHUIElement*> &children = const_cast<std::list<MHUIElement*> &>(cSelf->getChildren());
    std::list<MHUIElement*>::iterator itr = children.begin();
    for (; itr != children.end(); itr++) {
        rb_yield(MHUIElementBindings::Get()->getValue(*itr));
    }
    return rSelf;
}

#pragma mark Position Functions

VALUE MHUIElementBindings::AlwaysOnTop(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setAlwaysOnTop(true);
    return rSelf;
}
