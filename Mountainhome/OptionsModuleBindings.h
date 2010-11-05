/*
 *  OptionsModuleBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _OPTIONSMODULEBINDINGS_H_
#define _OPTIONSMODULEBINDINGS_H_

#include "OptionsModule.h"
#include "RubyBindings.h"

class OptionsModuleBindings : public RubyBindings<OptionsModule, false> {
public:
    static VALUE Apply(VALUE self);
    static VALUE GetValue(VALUE self, VALUE key);
    static VALUE PutValue(VALUE self, VALUE key, VALUE val);

public:
    OptionsModuleBindings();
    virtual ~OptionsModuleBindings() {}

};

#endif
