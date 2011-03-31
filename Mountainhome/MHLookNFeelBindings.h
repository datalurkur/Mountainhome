/*
 * MHLookNFeelBindings.h
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHLOOKNFEELBINDINGS_H_
#define _MHLOOKNFEELBINDINGS_H_

#include "MHLookNFeel.h"
#include "RubyBindings.h"

class MHLookNFeelBindings : public RubyBindings<MHLookNFeel, true> {
public:
    static VALUE CreateRectRenderable(VALUE self, VALUE w, VALUE h, VALUE matName);
    static VALUE CreateOffsetRectRenderable(VALUE self, VALUE w, VALUE h, VALUE xOff, VALUE yOff, VALUE matName);
    static VALUE CreateTextRenderable(VALUE self, VALUE font, VALUE color, VALUE text);

    static VALUE GetTextWidth(VALUE self, VALUE font, VALUE text);
    static VALUE GetTextHeight(VALUE self, VALUE font);

public:
    MHLookNFeelBindings();
    virtual ~MHLookNFeelBindings() {}
};

#endif
