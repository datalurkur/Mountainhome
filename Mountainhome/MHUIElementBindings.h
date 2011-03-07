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
#include "MHUIElement.h"

class MHUIElementBindings : public RubyBindings<MHUIElement, false> {
public:
    static void Mark(MHUIElement *cSelf);

    static VALUE Initialize(VALUE rSelf);

    // Child management.
    static VALUE AddChild(VALUE rSelf, VALUE child);
    static VALUE EachChild(VALUE rSelf);
    static VALUE DeleteChild(VALUE rSelf, VALUE child);
    static VALUE DeleteChildren(VALUE rSelf);

    // Parent management
    static VALUE GetParent(VALUE rSelf);
    static VALUE SetParent(VALUE rSelf, VALUE parent);

    // Position management.
    static VALUE XEquals(VALUE rSelf, VALUE value);
    static VALUE YEquals(VALUE rSelf, VALUE value);
    static VALUE WEquals(VALUE rSelf, VALUE value);
    static VALUE HEquals(VALUE rSelf, VALUE value);
    static VALUE GetX(VALUE rSelf);
    static VALUE GetY(VALUE rSelf);
    static VALUE GetW(VALUE rSelf);
    static VALUE GetH(VALUE rSelf);
    static VALUE AlwaysOnTop(VALUE rSelf);

public:
    MHUIElementBindings();
    virtual ~MHUIElementBindings() {}
};

#endif
