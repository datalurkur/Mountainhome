/*
 *  KeyboardBindings.h
 *  Mountainhome
 *
 *  Created by Paul on 3/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _KEYBOARDBINDINGS_H_
#define _KEYBOARDBINDINGS_H_
#include "RubyBindings.h"

/*! Provides the mapping of human readable key names to internally used key values. */
class KeyboardBindings {
    static KeyboardBindings* Instance;
    static KeyboardBindings* Get() { return Instance; }
public:
    /* Allows Keyboard.RETURN or Keyboard.CTRL syntax. */ 
    static VALUE Method_Missing(int argc, VALUE * argv, VALUE self);

public:
    KeyboardBindings();
    virtual ~KeyboardBindings() {}

private:
    VALUE _class;
    VALUE _rubyLookup; /* Reference to the Keyboard internal hash. */

};

#endif
