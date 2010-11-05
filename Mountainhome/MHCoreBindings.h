/*
 *  MHCoreBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHCOREBINDINGS_H_
#define _MHCOREBINDINGS_H_
#include "RubyBindings.h"
#include "MHCore.h"

class MHCoreBindings : public RubyBindings<MHCore, true> {
public:
    static VALUE Alloc(VALUE klass);

    /*! Stops the main application loop, causing clean up to begin and ending in the
     *  application's termination.
     * \param self The ruby object we're working on. */
    static VALUE Exit(VALUE self);

    /*! Registers a new state with MHCore.
     * \fixme This should probably be moved over to state RubyState or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE RegisterState(VALUE self, VALUE state, VALUE name);

    /*! Sets a new state as the current active state.
     * \fixme This should probably be moved over to state RubyState or something, to avoid
     *  the duplication. See the class fixme for more details.
     * \param self The ruby object we're working on. */
    static VALUE SetState(VALUE self, VALUE args);

    /*! Returns the window associated with the core. */
    static VALUE GetWindow(VALUE self);

    /*! Returns the window associated with the core. */
    static VALUE GetRenderContext(VALUE self);

    /*! Returns the options module. */
    static VALUE GetOptions(VALUE self);

    /*! Returns a list of loadable worlds based on a query string. */
    static VALUE GetPersonalDir(VALUE self);

    /*! Stops the menu music. */
    static VALUE StopMusic(VALUE self);

public:
    MHCoreBindings();
    virtual ~MHCoreBindings() {}

};

#endif
