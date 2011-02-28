/*
 * MHLookNFeelBindings.cpp
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project. All rights reserved.
 * 
 */

#include "MHLookNFeelBindings.h"
#include "MHUIElementBindings.h"

MHLookNFeelBindings::MHLookNFeelBindings()
: RubyBindings<MHLookNFeel, true>(
    rb_define_class("MHLookNFeel", rb_cObject),
    "MHLookNFeelBindings")
{
    rb_define_method(_class, "clear_renderables", RUBY_METHOD_FUNC(MHLookNFeelBindings::ClearRenderables), 1);
    rb_define_method(_class, "add_rect_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::AddRectRenderable), 4);
    rb_define_method(_class, "add_text_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::AddTextRenderable), 3);

    rb_define_alloc_func(_class, MHLookNFeelBindings::Alloc<MHLookNFeelBindings>);
}

VALUE MHLookNFeelBindings::ClearRenderables(VALUE rSelf, VALUE rElement) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cElement = MHUIElementBindings::Get()->getPointer(rElement);

    cSelf->clearRenderables(cElement);

    return rSelf;
}

VALUE MHLookNFeelBindings::AddRectRenderable(VALUE rSelf, VALUE rElement, VALUE rW, VALUE rH, VALUE rMatName) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cElement = MHUIElementBindings::Get()->getPointer(rElement);
    std::string cMatName = rb_string_value_cstr(&rMatName);

    cSelf->addRectRenderable(cElement, NUM2INT(rW), NUM2INT(rH), cMatName);

    return rSelf;
}

VALUE MHLookNFeelBindings::AddTextRenderable(VALUE rSelf, VALUE rElement, VALUE rFont, VALUE rText) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cElement = MHUIElementBindings::Get()->getPointer(rElement);
    std::string cText = rb_string_value_cstr(&rText);
    std::string cFont = rb_string_value_cstr(&rFont);

    cSelf->addTextRenderable(cElement, cFont, cText);

    return rSelf;
}
