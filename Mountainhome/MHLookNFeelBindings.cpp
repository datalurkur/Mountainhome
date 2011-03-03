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
    rb_define_method(_class, "add_text_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::AddTextRenderable), 4);

    rb_define_method(_class, "get_text_width", RUBY_METHOD_FUNC(MHLookNFeelBindings::GetTextWidth), 2);
    rb_define_method(_class, "get_text_height", RUBY_METHOD_FUNC(MHLookNFeelBindings::GetTextHeight), 1);

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

VALUE MHLookNFeelBindings::AddTextRenderable(VALUE rSelf, VALUE rElement, VALUE rFont, VALUE rColor, VALUE rText) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    MHUIElement *cElement = MHUIElementBindings::Get()->getPointer(rElement);

    std::string cText = rb_string_value_cstr(&rText);
    std::string cFont = rb_string_value_cstr(&rFont);

    Color4 cColor;
    for(int i=0; i < RARRAY_LEN(rColor) && i < 4; i++) {
        cColor[i] = NUM2DBL(rb_ary_entry(rColor, i));
    }

    cSelf->addTextRenderable(cElement, cFont, cColor, cText);

    return rSelf;
}

VALUE MHLookNFeelBindings::GetTextWidth(VALUE rSelf, VALUE rFont, VALUE rText) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    std::string cFont = rb_string_value_cstr(&rFont);
    std::string cText = rb_string_value_cstr(&rText);

    int width = cSelf->getTextWidth(cFont, cText);
    return INT2NUM(width);
}

VALUE MHLookNFeelBindings::GetTextHeight(VALUE rSelf, VALUE rFont) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    std::string cFont = rb_string_value_cstr(&rFont);

    int height = cSelf->getTextHeight(cFont);
    return INT2NUM(height);
}
