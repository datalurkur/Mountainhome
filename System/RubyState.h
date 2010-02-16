/*
 *  RubyState.h
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYSTATE_H_
#define _RUBYSTATE_H_
#include <Engine/ParentState.h>
#include "RubyBindings.h"

/*! Creates a Ruby class called State which maps to a C++ ParentState. This mapping was
 *  chosen to avoid the need for creating a State AND ParentState mapping. The main
 *  benefit of having these two classes is compile time notices of unwanted calls to
 *  ParentState functionality, but we don't get that in Ruby, so we might as well just
 *  have a single State class. It should also be noted that when a ruby object calls
 *  pushState or setActiveState, the associated setup call should get all of its arguments
 *  in a nice ruby array, so we don't have to worry about va_list weirdness. */
class RubyState : public ParentState {
public:
    /*! Creates the ruby land State class, which maps directly to a C++ RubyState object.
     *  It also sets up the various interned values needed by the class.. */
    static void SetupBindings();

    /*! This class keeps track of the mapping between ruby objects and c++ objects and
     *  allows other objects to do a lookup if they need to */
    static RubyState* GetState(VALUE robj);

public:
    /*! The map used internally to track RubyState objects */
    typedef std::map<VALUE, RubyState*> RubyObjectMap;

    /*! Contains a blank impl of the setup method so inheritting classes won't get method
     *  missing errors if they don't define one.
     * \param self The ruby object we're working on. */
    static VALUE Setup(VALUE self);

    /*! Contains a blank impl of the teardown method so inheritting classes won't get
     *  method missing errors if they don't define one.
     * \param self The ruby object we're working on. */
    static VALUE Teardown(VALUE self);

    /*! Creates a matching c++ object and stores it in a static map for later lookup.
     * \param self The ruby object we're working on. */
    static VALUE Initialize(VALUE self);

protected:
    static RubyObjectMap Objects; /*!< Mapping of ruby objects to c++ objects. */
    static VALUE Class;           /*!< Ruby's State class value.               */
 
    static VALUE TeardownMethod;  /*!< The teardown method value. */
    static VALUE SetupMethod;     /*!< The setup method value.    */
    static VALUE UpdateMethod;    /*!< The update method value.   */

public:
    /*! Creates a new RubyState instance and stores a reference to the matching ruby
     *  object. This VALUE is used to call down into ruby as needed. */
    RubyState(VALUE robj);

    /*! Typical d'tor */
    virtual ~RubyState();

    /*! Calls down into the ruby object's 'setup' method. The args array here should be full
     *  of ruby VALUEs, which can be passed directly into the ruby object.
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
