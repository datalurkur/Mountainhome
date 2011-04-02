/*
 *  LoggerBindings.h
 *  System
 *
 *  Created by loch on 2/22/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _LOGGERBINDINGS_H_
#define _LOGGERBINDINGS_H_
#include <Base/Logger.h>
#include "RubyBindings.h"

/*! This class acts as a wrapper around the c++ level logger, allowing ruby to log using
 *  the same mechanism as c++. This helps integrate the two code layers and keep things
 *  nice and pretty.
 * \todo Get the file and line printed to be based on the current ruby file and line. */
class LoggerBindings {
public:
    /*! Increases the indentation level. */
    static VALUE IncrementIndent(VALUE self);

    /*! Decreases the indentation level. */
    static VALUE DecrementIndent(VALUE self);

    /*! Logs an error message with the WarningMessage level */
    static VALUE LogWarn(VALUE self, VALUE str);

    /*! Logs an error message with the InfoMessage level */
    static VALUE LogInfo(VALUE self, VALUE str);

    /*! Logs an error message with the ErrorMessage level */
    static VALUE LogError(VALUE self, VALUE str);

    /*! Logs an error message with the DebugMessage level */
    static VALUE LogDebug(VALUE self, VALUE str);

    /*! Channel method passthroughs. */
    static VALUE EnableChannel (VALUE self, VALUE channel);
    static VALUE DisableChannel(VALUE self, VALUE channel);
    static VALUE EnableAllChannels(VALUE self);
    static VALUE DisableAllChannels(VALUE self);

protected:
    static VALUE Log(VALUE str, LogStream::LogType level, LogChannel channel);

public:
    LoggerBindings();
    virtual ~LoggerBindings() {}

private:
    VALUE _object;
    VALUE _class;

};

#endif
