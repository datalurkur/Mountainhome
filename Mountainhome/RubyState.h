/*
 *  RubyState.h
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
 *  RubyState is created and when the State object is made active, the RubyState
 *  object is what is actually installed in c++. From here, any functions called on the
 *  RubyState object is passed down to the matching method in the State object.
 * \note This mapping was chosen to avoid the need for creating a State AND ParentState
 *  mapping. The main benefit of having these two classes is compile time notices of
 *  unwanted calls to ParentState functionality, but we don't get that in Ruby, so we
 *  might as well just have a single State class.
 * \note It should also be noted that when a ruby object sets a new active state, the
 *  associated setup call should get all of its arguments in a nice ruby array, so we
 *  don't have to worry about va_list weirdness. */
class RubyState : public ParentState, public RubyBindings<RubyState, false> {
public:
#pragma mark RubyState ruby bindings
    /*! Creates the ruby land State class, which maps directly to a C++ RubyState
     *  object. It also sets up the various interned values needed by the RubyState
     *  objects to call down into their paired ruby objects. */
    static void SetupBindings();

    /*! The allocator function for RubyStates. */
    static VALUE Alloc(VALUE klass);

protected:
    static VALUE TeardownMethod;  /*!< The teardown method value. */
    static VALUE SetupMethod;     /*!< The setup method value.    */
    static VALUE UpdateMethod;    /*!< The update method value.   */
    
    /* Event Handler method values. */
    static VALUE KeyTypedMethod;
    static VALUE KeyPressedMethod;
    static VALUE KeyReleasedMethod;
    static VALUE MouseMovedMethod;
    static VALUE MouseClickedMethod;
    static VALUE MousePressedMethod;
    static VALUE MouseReleasedMethod;

public:
#pragma mark RubyState declarations
    /*! Creates a new RubyState instance and stores a reference to the matching ruby
     *  State object, allowing it to pass setup/update/teardown calls up to ruby.
     * \param robj The VALUE representing the matching ruby State object. */
    RubyState();

    /*! Typical d'tor */
    virtual ~RubyState();

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
    
    
#pragma mark Event Handlers

    /*! Delegates to the currently active state.
     * \seealso State::keyTyped */
    virtual void keyTyped(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::keyPressed */
    virtual void keyPressed(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::keyReleased */
    virtual void keyReleased(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseMoved */
    virtual void mouseMoved(MouseMotionEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseClicked */
    virtual void mouseClicked(MouseButtonEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mousePressed */
    virtual void mousePressed(MouseButtonEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseReleased */
    virtual void mouseReleased(MouseButtonEvent *event);

private:
    VALUE _rubyObject; /*!< Reference to the matching ruby land object. */

};

#endif
