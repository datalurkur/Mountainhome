/*
 *  Mountainhome.h
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#ifndef _MOUNTAINHOME_H_
#define _MOUNTAINHOME_H_
#include <Base/Singleton.h>
#include <Engine/DefaultCore.h>
#include "RubyBindings.h"

/*! Mountainhome is the main application class which inherits from the Engine's
 *  DefaultCore, making it entirely central to the application. It provides the entry
 *  point for the application and handles all initial setup and final shutdown and
 *  delegates where the execution flow goes as it is handed over by the engine (update,
 *  display, any form of input, etc... all go through this object first). This class also
 *  defines ruby binding for itself, which gives us access to the root of the application
 *  from ruby.
 * \note SetupBindings installs a global $mountainhome variable in ruby.
 * \fixme Sadly all of the state interaction code here is a duplicate of what we have in
 *  RubyStateProxy. I'm not sure what the best way to get around this is, though (perhaps just
 *  reference RubyStateProxy methods in here? Maybe we can do some scary casting nonsense). For
 *  now I'm just leaving in the duplicate code. */
class Mountainhome : public DefaultCore, public Singleton<Mountainhome> {
public:
#pragma mark Mountainhome ruby bindings
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
     * \fixme This should probably be moved over to state RubyStateProxy or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE RegisterState(VALUE self, VALUE state, VALUE name);

    /*! Sets a new state as the current active state.
     * \fixme This should probably be moved over to state RubyStateProxy or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE SetState(VALUE self, VALUE name);

protected:
    static VALUE Object; /*!< The reference to the ruby land Mountainhome object. */
    static VALUE Class;  /*!< The reference to the ruby land Mountainhome class.  */

#pragma mark Mountainhome static declarations
public:
    static const std::string GameStateID;
    static Window *GetWindow();

public:
#pragma mark Mountainhome declarations
    virtual void keyPressed(KeyEvent *event);
    virtual void setup(va_list args);

protected:
    Mountainhome();
    virtual ~Mountainhome();
    template <class T> friend class Singleton;

    // To allow MountainhomeBinding to call registerState
    friend class RubyMountainhome;

};
#endif
