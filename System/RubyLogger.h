/*
 *  RubyLogger.h
 *  System
 *
 *  Created by loch on 2/22/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYLOGGER_H_
#define _RUBYLOGGER_H_
#include "RubyBindings.h"

class RubyLogger {
public:
    #pragma mark RubyLogger ruby bindings
    /*! Creates the ruby land MHLogger class and installs a global instance of it. */
    static void SetupBindings();

    static VALUE LogWarn(VALUE self, VALUE str);
    static VALUE LogInfo(VALUE self, VALUE str);
    static VALUE LogError(VALUE self, VALUE str);
    static VALUE LogDebug(VALUE self, VALUE str);

protected:
    static VALUE Object; /*!< The reference to the ruby land Mountainhome object. */
    static VALUE Class;  /*!< The reference to the ruby land Mountainhome class.  */

};

#endif
