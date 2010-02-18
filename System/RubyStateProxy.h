/*
 *  RubyStateProxy.h
 *  System
 *
 *  Created by loch on 2/18/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _STATEPROXY_H_
#define _STATEPROXY_H_
#include <Engine/ParentState.h>
#include "RubyBindings.h"

/*! This class simultaneously defines a ruby level State class (that acts as a match for
 *  the c++ State class) and acts as a c++ level proxy for instances of this ruby level
 *  State class. When a ruby script creates a new instance of a State, a matching
 *  RubyStateProxy is created and when the State object is made active, the RubyStateProxy
 *  object is what is actually installed in c++. From here, any functions called on the
 *  RubyStateProxy object is passed down to the matching method in the State object.
 * \note This mapping was chosen to avoid the need for creating a State AND ParentState
 *  mapping. The main benefit of having these two classes is compile time notices of
 *  unwanted calls to ParentState functionality, but we don't get that in Ruby, so we
 *  might as well just have a single State class.
 * \note It should also be noted that when a ruby object sets a new active state, the
 *  associated setup call should get all of its arguments in a nice ruby array, so we
 *  don't have to worry about va_list weirdness. */
class RubyStateProxy : public ParentState, public ManyObjectBinding<RubyStateProxy> {
public:
#pragma mark StateObject ruby bindings
    /*! Creates the ruby land State class, which maps directly to a C++ RubyStateProxy
     *  object. It also sets up the various interned values needed by the RubyStateProxy
     *  objects to call down into their paired ruby objects. */
    static void SetupBindings();

    /*! Contains a blank impl of the setup method so inheritting classes won't get method
     *  missing errors if they don't define one themselves.
     * \param self The ruby object we're working on. */
    static VALUE Setup(VALUE self);

    /*! Contains a blank impl of the teardown method so inheritting classes won't get
     *  method missing errors if they don't define one themselves.
     * \param self The ruby object we're working on. */
    static VALUE Teardown(VALUE self);

    /*! Creates a matching c++ object and stores it in a static map for later lookup.
     * \param self The ruby object we're working on. */
    static VALUE Initialize(VALUE self);

protected:
    static VALUE TeardownMethod;  /*!< The teardown method value. */
    static VALUE SetupMethod;     /*!< The setup method value.    */
    static VALUE UpdateMethod;    /*!< The update method value.   */

public:
#pragma mark StateObject declarations
    /*! Creates a new RubyStateProxy instance and stores a reference to the matching ruby
     *  State object, allowing it to pass setup/update/teardown calls up to ruby.
     * \param robj The VALUE representing the matching ruby State object. */
    RubyStateProxy(VALUE robj);

    /*! Typical d'tor */
    virtual ~RubyStateProxy();

    /*! Calls down into the ruby object's 'setup' method. The args array here should be
     *  full of ruby VALUEs, which can be passed directly into the ruby object.
     * \params args A varargs list of ruby VALUES. */
    virtual void setup(va_list args);

    /*! Calls directly into the ruby objects 'teardown' method. */
    virtual void teardown();

    /*! Calls directly into the ruby object's 'update' method, passing the elapsed time in
     *  milliseconds along the way.
     * \param elapsed The elapsed time in milliseconds. */
    virtual void update(int elapsed);

private:
    VALUE _rubyObject; /*!< Reference to the matching ruby land object. */

};

#endif
