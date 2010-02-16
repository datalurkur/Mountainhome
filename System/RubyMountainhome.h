/*
 *  RubyMountainhome.h
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYMOUNTAINHOME_H_
#define _RUBYMOUNTAINHOME_H_
#include "RubyBindings.h"

/*! This is our ruby binding for the main Mountainhome object, which gives us access to
 *  the root of the application from ruby land. Because this is a mapping to a singleton
 *  object, we don't need to implement any actual functionality in this class, leaving
 *  this as nothing more than a binding layer.
 * \note SetupBindings installs a global $mountainhome variable in ruby.
 * \fixme Sadly all of the state interaction code here is a duplicate of what we have in
 *  RubyState. I'm not sure what the best way to get around this is, though (perhaps just
 *  reference RubyState methods in here? Maybe we can do some scary casting nonsense). For
 *  now I'm just leaving in the duplicate code. */
class RubyMountainhome {
public:
    /*! Sets up the Mountainhome class in ruby land, creates a new instance of it and
     *  binds it to a global variable for general use.
     * \todo Right now, it just makes a normal class and creates a new object. I'd much
     *  rather make a singleton class and call instance, but for some reason the code
     *  blows up on me. I've left it there, commented out for later inspection. */
    static void SetupBindings();

    /*! Stops the main application loop, causing clean up to begin and ending in the
     *  application's termination.
     * \param self The ruby object we're working on. */
    static VALUE StopMainLoop(VALUE self);

    /*! Registers a new state with Mountainhome.
     * \fixme This should probably be moved over to state RubyState or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE RegisterState(VALUE self, VALUE state, VALUE name);

    /*! Sets a new state as the current active state.
     * \fixme This should probably be moved over to state RubyState or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE SetState(VALUE self, VALUE name);

protected:
    static VALUE Object; /*!< The reference to the ruby land Mountainhome object. */
    static VALUE Class;  /*!< The reference to the ruby land Mountainhome class.  */

};

#endif
