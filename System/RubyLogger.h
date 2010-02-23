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

/*! This class acts as a wrapper around the c++ level logger, allowing ruby to log using
 *  the same mechanism as c++. This helps integrate the two code layers and keep things
 *  nice and pretty.
 * \todo Get the file and line printed to be based on the current ruby file and line. */
class RubyLogger {
public:
    #pragma mark RubyLogger ruby bindings
    /*! Creates the ruby land MHLogger class and installs a global instance of it. */
    static void SetupBindings();

    /*! Logs an error message with the WarningMessage level */
    static VALUE LogWarn(VALUE self, VALUE str);

    /*! Logs an error message with the InfoMessage level */
    static VALUE LogInfo(VALUE self, VALUE str);

    /*! Logs an error message with the ErrorMessage level */
    static VALUE LogError(VALUE self, VALUE str);

    /*! Logs an error message with the DebugMessage level */
    static VALUE LogDebug(VALUE self, VALUE str);

protected:
    static VALUE Object; /*!< The reference to the ruby land Mountainhome object. */
    static VALUE Class;  /*!< The reference to the ruby land Mountainhome class.  */

};

#endif
