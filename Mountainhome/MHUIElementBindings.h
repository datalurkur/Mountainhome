/*
 *  MHUIElementBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHUIELEMENTBINDINGS_H_
#define _MHUIELEMENTBINDINGS_H_
#include "RubyBindings.h"
#include "EntityBindings.h"
#include "MHUIElement.h"

class MHUIElementBindings : public RubyBindings<MHUIElement, true> {
public:
    static void Mark(MHUIElement *cSelf);

    static VALUE Initialize(VALUE rSelf, VALUE name, VALUE manager, VALUE mat, VALUE rFontName);
    static VALUE AddChild(VALUE rSelf, VALUE child);
    static VALUE EachChild(VALUE rSelf);
    static VALUE CullChild(VALUE rSelf, VALUE child);
    static VALUE CullChildren(VALUE rSelf);

    // Setter Bindings
    static VALUE SetText(VALUE rSelf, VALUE text);
    static VALUE WEquals(VALUE rSelf, VALUE value);
    static VALUE HEquals(VALUE rSelf, VALUE value);
    static VALUE XOffsetEquals(VALUE rSelf, VALUE value);
    static VALUE YOffsetEquals(VALUE rSelf, VALUE value);

    // Accessor Bindings
    static VALUE GetText(VALUE rSelf);
    static VALUE TextWidth(VALUE rSelf);
    static VALUE TextHeight(VALUE rSelf);

    static VALUE W(VALUE rSelf);
    static VALUE H(VALUE rSelf);
    static VALUE XOffset(VALUE rSelf);
    static VALUE YOffset(VALUE rSelf);
    
    // General config bindings
    static VALUE SetDimensions(VALUE rSelf, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE SetOffset(VALUE rSelf, VALUE x, VALUE y);
    static VALUE SetBorder(VALUE rSelf, VALUE border);
    static VALUE Resize(VALUE rSelf, VALUE x, VALUE y);
    static VALUE AlwaysOnTop(VALUE rSelf);

public:
    MHUIElementBindings();
    virtual ~MHUIElementBindings() {}

};

#endif
