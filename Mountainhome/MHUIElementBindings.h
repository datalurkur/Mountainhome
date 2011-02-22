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

    static VALUE Initialize(VALUE rSelf);

    // Child management.
    static VALUE AddChild(VALUE rSelf, VALUE child);
    static VALUE EachChild(VALUE rSelf);
    static VALUE DeleteChild(VALUE rSelf, VALUE child);
    static VALUE DeleteChildren(VALUE rSelf);

    static VALUE AlwaysOnTop(VALUE rSelf);

public:
    MHUIElementBindings();
    virtual ~MHUIElementBindings() {}

};

#endif
