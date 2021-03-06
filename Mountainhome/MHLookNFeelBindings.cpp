/*
 * MHLookNFeelBindings.cpp
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project. All rights reserved.
 * 
 */

#include "MHLookNFeelBindings.h"
#include "RenderableBindings.h"

MHLookNFeelBindings::MHLookNFeelBindings()
: RubyBindings<MHLookNFeel, true>(
    rb_define_class("MHLookNFeel", rb_cObject),
    "MHLookNFeelBindings")
{
    rb_define_method(_class, "create_rect_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::CreateRectRenderable), 3);
    rb_define_method(_class, "create_offset_rect_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::CreateOffsetRectRenderable), 5);
    rb_define_method(_class, "create_text_renderable", RUBY_METHOD_FUNC(MHLookNFeelBindings::CreateTextRenderable), 3);

    rb_define_method(_class, "get_text_width", RUBY_METHOD_FUNC(MHLookNFeelBindings::GetTextWidth), 2);
    rb_define_method(_class, "get_text_height", RUBY_METHOD_FUNC(MHLookNFeelBindings::GetTextHeight), 1);
    rb_define_method(_class, "split_text_at", RUBY_METHOD_FUNC(MHLookNFeelBindings::SplitTextAt), 3);

    rb_define_alloc_func(_class, MHLookNFeelBindings::Alloc<MHLookNFeelBindings>);
}

VALUE MHLookNFeelBindings::CreateOffsetRectRenderable(VALUE rSelf, VALUE rW, VALUE rH, VALUE rXOff, VALUE rYOff, VALUE rMatName) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    std::string cMatName = rb_string_value_cstr(&rMatName);

    Renderable *cRenderable = cSelf->createRectRenderable(NUM2INT(rW), NUM2INT(rH), NUM2INT(rXOff), NUM2INT(rYOff), cMatName);
    return NEW_RUBY_OBJECT(RenderableBindings, cRenderable);
}

VALUE MHLookNFeelBindings::CreateRectRenderable(VALUE rSelf, VALUE rW, VALUE rH, VALUE rMatName) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    std::string cMatName = rb_string_value_cstr(&rMatName);

    Renderable *cRenderable = cSelf->createRectRenderable(NUM2INT(rW), NUM2INT(rH), 0, 0, cMatName);
    return NEW_RUBY_OBJECT(RenderableBindings, cRenderable);
}

VALUE MHLookNFeelBindings::CreateTextRenderable(VALUE rSelf, VALUE rFont, VALUE rColor, VALUE rText) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);

    std::string cText = rb_string_value_cstr(&rText);
    std::string cFont = rb_string_value_cstr(&rFont);

    Color4 cColor;
    for(int i=0; i < RARRAY_LEN(rColor) && i < 4; i++) {
        cColor[i] = NUM2DBL(rb_ary_entry(rColor, i));
    }

    Renderable *cRenderable = cSelf->createTextRenderable(cFont, cColor, cText);
    return NEW_RUBY_OBJECT(RenderableBindings, cRenderable);
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

VALUE MHLookNFeelBindings::SplitTextAt(VALUE rSelf, VALUE rFont, VALUE rText, VALUE rWidth) {
    MHLookNFeel *cSelf = Get()->getPointer(rSelf);
    std::string cFont = rb_string_value_cstr(&rFont);
    std::string cText = rb_string_value_cstr(&rText);

    std::vector<std::string> snippets;
    cSelf->splitTextAt(cFont, cText, NUM2INT(rWidth), snippets);

    std::vector<std::string>::iterator itr = snippets.begin();
    VALUE rSnippets = rb_ary_new();
    for(; itr != snippets.end(); itr++) {
        rb_ary_push(rSnippets, rb_str_new2((*itr).c_str()));
    }
    return rSnippets;
}
