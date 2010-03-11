/*
 *  RubyKeyboard.h
 *  Mountainhome
 *
 *  Created by Paul on 3/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYKEYBOARD_H_
#define _RUBYKEYBOARD_H_
#include "RubyBindings.h"

class RubyKeyboard {
public:
#pragma mark declarations
    /* Creates the ruby land Keyboard class, and sets up the key enumerations. */
    static void SetupBindings();
    
    /* Allows Keyboard.RETURN or Keyboard.CTRL syntax. */ 
    static VALUE Method_Missing(int argc, VALUE * argv, VALUE self);
    
private:
    static VALUE Class; /* Reference to the matching ruby land class. */
    static VALUE RubyLookup; /* Reference to the Keyboard internal hash. */
};

#endif