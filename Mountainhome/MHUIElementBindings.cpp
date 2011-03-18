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

MHUIElementBindings::MHUIElementBindings()
: RubyBindings<MHUIElement, false>(
    rb_define_class("MHUIElement", rb_cObject),
    "MHUIElementBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHUIElementBindings::Initialize), 0);

    rb_define_method(_class, "add_child", RUBY_METHOD_FUNC(MHUIElementBindings::AddChild), 1);
    rb_define_method(_class, "delete_child", RUBY_METHOD_FUNC(MHUIElementBindings::DeleteChild), 1);
    rb_define_method(_class, "delete_children", RUBY_METHOD_FUNC(MHUIElementBindings::DeleteChildren), 0);
    rb_define_method(_class, "each_child", RUBY_METHOD_FUNC(MHUIElementBindings::EachChild), 0);

    rb_define_method(_class, "parent", RUBY_METHOD_FUNC(MHUIElementBindings::GetParent), 0);
    rb_define_method(_class, "parent=", RUBY_METHOD_FUNC(MHUIElementBindings::SetParent), 1);

    rb_define_method(_class, "x=", RUBY_METHOD_FUNC(MHUIElementBindings::XEquals), 1);
    rb_define_method(_class, "y=", RUBY_METHOD_FUNC(MHUIElementBindings::YEquals), 1);
    rb_define_method(_class, "w=", RUBY_METHOD_FUNC(MHUIElementBindings::WEquals), 1);
    rb_define_method(_class, "h=", RUBY_METHOD_FUNC(MHUIElementBindings::HEquals), 1);

    rb_define_method(_class, "x", RUBY_METHOD_FUNC(MHUIElementBindings::GetX), 0);
    rb_define_method(_class, "y", RUBY_METHOD_FUNC(MHUIElementBindings::GetY), 0);
    rb_define_method(_class, "w", RUBY_METHOD_FUNC(MHUIElementBindings::GetW), 0);
    rb_define_method(_class, "h", RUBY_METHOD_FUNC(MHUIElementBindings::GetH), 0);

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
    bool ret = cSelf->deleteChild(cChild);
    return ret ? Qtrue : Qfalse;
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

#pragma mark Parent Functions

VALUE MHUIElementBindings::GetParent(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *parent = cSelf->getParent();
    return (parent == 0) ? MHUIElementBindings::Get()->getValue(parent) : Qnil;
}

VALUE MHUIElementBindings::SetParent(VALUE rSelf, VALUE rParent) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cParent = Get()->getPointer(rParent);
    cParent->addChild(cSelf);
    return rSelf;
}

#pragma mark Position Functions

VALUE MHUIElementBindings::XEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setX(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::YEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setY(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::WEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setW(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::HEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setH(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::GetX(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getX());
}

VALUE MHUIElementBindings::GetY(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getY());
}

VALUE MHUIElementBindings::GetW(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getW());
}

VALUE MHUIElementBindings::GetH(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getH());
}

VALUE MHUIElementBindings::AlwaysOnTop(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setAlwaysOnTop(true);
    return rSelf;
}
