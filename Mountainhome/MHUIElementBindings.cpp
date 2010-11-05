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
#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/FontManager.h>

MHUIElementBindings::MHUIElementBindings()
: RubyBindings<MHUIElement, true>(
    rb_define_class("MHUIElement", rb_cObject),
    "MHUIElementBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHUIElementBindings::Initialize), 4);
    rb_define_method(_class, "add_child", RUBY_METHOD_FUNC(MHUIElementBindings::AddChild), 1);
    rb_define_method(_class, "cull_child", RUBY_METHOD_FUNC(MHUIElementBindings::CullChild), 1);
    rb_define_method(_class, "cull_children", RUBY_METHOD_FUNC(MHUIElementBindings::CullChildren), 0);
    rb_define_method(_class, "each_child", RUBY_METHOD_FUNC(MHUIElementBindings::EachChild), 0);

    rb_define_method(_class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElementBindings::SetDimensions), 4);
    rb_define_method(_class, "set_offset", RUBY_METHOD_FUNC(MHUIElementBindings::SetOffset), 2);
    rb_define_method(_class, "set_border", RUBY_METHOD_FUNC(MHUIElementBindings::SetBorder), 1);
    rb_define_method(_class, "set_position", RUBY_METHOD_FUNC(MHUIElementBindings::SetPosition), 2);
    rb_define_method(_class, "move_relative", RUBY_METHOD_FUNC(MHUIElementBindings::MoveRelative), 2);
    rb_define_method(_class, "resize", RUBY_METHOD_FUNC(MHUIElementBindings::Resize), 2);
    rb_define_method(_class, "always_on_top", RUBY_METHOD_FUNC(MHUIElementBindings::AlwaysOnTop), 0);
    rb_define_method(_class, "text=", RUBY_METHOD_FUNC(MHUIElementBindings::SetText), 1);
    rb_define_method(_class, "text", RUBY_METHOD_FUNC(MHUIElementBindings::GetText), 0);
    rb_define_method(_class, "text_width", RUBY_METHOD_FUNC(MHUIElementBindings::TextWidth), 0);
    rb_define_method(_class, "text_height", RUBY_METHOD_FUNC(MHUIElementBindings::TextHeight), 0);
    rb_define_method(_class, "x=", RUBY_METHOD_FUNC(MHUIElementBindings::XEquals), 1);
    rb_define_method(_class, "y=", RUBY_METHOD_FUNC(MHUIElementBindings::YEquals), 1);
    rb_define_method(_class, "x", RUBY_METHOD_FUNC(MHUIElementBindings::X), 0);
    rb_define_method(_class, "y", RUBY_METHOD_FUNC(MHUIElementBindings::Y), 0);
    rb_define_method(_class, "w=", RUBY_METHOD_FUNC(MHUIElementBindings::WEquals), 1);
    rb_define_method(_class, "h=", RUBY_METHOD_FUNC(MHUIElementBindings::HEquals), 1);
    rb_define_method(_class, "w", RUBY_METHOD_FUNC(MHUIElementBindings::W), 0);
    rb_define_method(_class, "h", RUBY_METHOD_FUNC(MHUIElementBindings::H), 0);
    rb_define_method(_class, "x_offset=", RUBY_METHOD_FUNC(MHUIElementBindings::XOffsetEquals), 1);
    rb_define_method(_class, "y_offset=", RUBY_METHOD_FUNC(MHUIElementBindings::YOffsetEquals), 1);
    rb_define_method(_class, "x_offset", RUBY_METHOD_FUNC(MHUIElementBindings::XOffset), 0);
    rb_define_method(_class, "y_offset", RUBY_METHOD_FUNC(MHUIElementBindings::YOffset), 0);
    rb_define_alloc_func(_class, MHUIElementBindings::Alloc<MHUIElementBindings>);
}

void MHUIElementBindings::Mark(MHUIElement *cSelf) {
    std::list<MHUIElement*> &children = const_cast<std::list<MHUIElement*> &>(cSelf->getChildren());

    std::list<MHUIElement*>::iterator itr = children.begin();
    for (; itr != children.end(); itr++) {
        rb_gc_mark(MHUIElementBindings::Get()->getValue(*itr));
    }
}

VALUE MHUIElementBindings::Initialize(VALUE rSelf, VALUE rName, VALUE rManager, VALUE rMatName, VALUE rFontName) {
    MHUIManager *cManager = MHUIManagerBindings::Get()->getPointer(rManager);
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    std::string cName     = rb_string_value_cstr(&rName);
    std::string cMatName  = rb_string_value_cstr(&rMatName);
    std::string cFontName = rb_string_value_cstr(&rFontName);

    Material *cMat = NULL;
    if (cMatName.size() > 0) {
        cMat = cManager->getMaterialManager()->getOrLoadResource(cMatName);
    }

    Font *cFont = cManager->getFont();
    if (cFontName.size() > 0) {
        cFont = cManager->getFontManager()->getOrLoadResource(cFontName);
    }

    cSelf->initialize(cName, cManager, cMat, cFont);
    
    return rSelf;
}

#pragma mark Child Functions

VALUE MHUIElementBindings::AddChild(VALUE rSelf, VALUE rChild) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cChild = Get()->getPointer(rChild);
    cSelf->addChild(cChild);
    return rSelf;
}

VALUE MHUIElementBindings::CullChild(VALUE rSelf, VALUE rChild) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cChild = Get()->getPointer(rChild);
    cSelf->cullChild(cChild);
    return rSelf;
}

VALUE MHUIElementBindings::CullChildren(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->cullChildren();
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

#pragma mark Text Functions

VALUE MHUIElementBindings::SetText(VALUE rSelf, VALUE rText) {
    std::string cText = rb_string_value_cstr(&rText);
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setText(cText);
    return rSelf;
}

VALUE MHUIElementBindings::GetText(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return rb_str_new2(cSelf->getText().c_str());
}

VALUE MHUIElementBindings::TextWidth(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getTextWidth());
}

VALUE MHUIElementBindings::TextHeight(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getTextHeight());
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

VALUE MHUIElementBindings::X(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getLocalPosition().x);
}

VALUE MHUIElementBindings::Y(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getLocalPosition().y);
}

VALUE MHUIElementBindings::WEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setWidth(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::HEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setHeight(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::W(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHUIElementBindings::H(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHUIElementBindings::XOffsetEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setXOffset(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::YOffsetEquals(VALUE rSelf, VALUE value) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setYOffset(NUM2INT(value));
    return value;
}

VALUE MHUIElementBindings::XOffset(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getXOffset());
}

VALUE MHUIElementBindings::YOffset(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getYOffset());
}

VALUE MHUIElementBindings::SetDimensions(VALUE rSelf, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    cSelf->setWidth(NUM2INT(w));
    cSelf->setHeight(NUM2INT(h));
    return rSelf;
}

VALUE MHUIElementBindings::SetOffset(VALUE rSelf, VALUE x, VALUE y) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setXOffset(NUM2INT(x));
    cSelf->setYOffset(NUM2INT(y));
    return rSelf;
}

VALUE MHUIElementBindings::SetBorder(VALUE rSelf, VALUE border) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setBorder(NUM2INT(border));
    return rSelf;
}

VALUE MHUIElementBindings::SetPosition(VALUE rSelf, VALUE x, VALUE y) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    return rSelf;
}

VALUE MHUIElementBindings::MoveRelative(VALUE rSelf, VALUE x, VALUE y) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->moveRelative(Vector3(NUM2INT(x), NUM2INT(y), 0.0));
    return rSelf;
}

VALUE MHUIElementBindings::Resize(VALUE rSelf, VALUE x, VALUE y) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setWidth(NUM2INT(x));
    cSelf->setHeight(NUM2INT(y));
    return rSelf;
}

VALUE MHUIElementBindings::AlwaysOnTop(VALUE rSelf) {
    MHUIElement *cSelf = Get()->getPointer(rSelf);
    cSelf->setAlwaysOnTop(true);
    return rSelf;
}
