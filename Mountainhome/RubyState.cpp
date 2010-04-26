/*
 *  RubyState.cpp
 *  System
 *
 *  Created by loch on 2/18/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyState.h"
#include <Base/Logger.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE RubyState::TeardownMethod = NULL;
VALUE RubyState::SetupMethod    = NULL;
VALUE RubyState::UpdateMethod   = NULL;

VALUE RubyState::KeyTypedMethod      = NULL;
VALUE RubyState::KeyPressedMethod    = NULL;
VALUE RubyState::KeyReleasedMethod   = NULL;
VALUE RubyState::MouseMovedMethod    = NULL;
VALUE RubyState::MouseClickedMethod  = NULL;
VALUE RubyState::MousePressedMethod  = NULL;
VALUE RubyState::MouseReleasedMethod = NULL;

VALUE RubyState::Alloc(VALUE klass) {
    RubyState *cState = new RubyState();
    VALUE rState = CreateBindingPairWithClass(klass, RubyState, cState);
    cState->_rubyObject = rState;
    return rState;
}

void RubyState::SetupBindings() {
    TeardownMethod = rb_intern("teardown");
    UpdateMethod   = rb_intern("update");
    SetupMethod    = rb_intern("setup");

    /*  If mouse_moved(4 args) has the same name as mouse_moved(1 arg), this funcall
        calls the 1 arg mouse_moved. This occurs whether the mouse_moved(4 args) is
        defined in either State (which MenuState inherits from) OR if it's defined
        in the StateEventCreator module. The current XTREME HACK is to rename the 4-arg
        method to mouse_moved0. Removing this hack currently causes a Ruby exception
        with NO BACKTRACE.
    */
    KeyTypedMethod      = rb_intern("key_typed0");
    KeyPressedMethod    = rb_intern("key_pressed0");
    KeyReleasedMethod   = rb_intern("key_released0");
    MouseMovedMethod    = rb_intern("mouse_moved0");
    MouseClickedMethod  = rb_intern("mouse_clicked0");
    MousePressedMethod  = rb_intern("mouse_pressed0");
    MouseReleasedMethod = rb_intern("mouse_released0");

    Class = rb_define_class("MHState", rb_cObject);
    rb_define_alloc_func(Class, RubyState::Alloc);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
RubyState::RubyState(): _rubyObject(0) {}
RubyState::~RubyState() {}

void RubyState::update(int elapsed) {
    if(rb_respond_to(_rubyObject, UpdateMethod)) {
        rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed));
    }
}

void RubyState::setup(va_list cArgs) {
    if(rb_respond_to(_rubyObject, SetupMethod)) {
        VALUE rArgsArray = va_arg(cArgs, VALUE);
        int argc = RARRAY_LEN(rArgsArray);

        VALUE argv[argc];
        for (int i = 0; i < argc; i++) {
            argv[i] = rb_ary_shift(rArgsArray);
        }

        rb_funcall2(_rubyObject, SetupMethod, argc, argv);
    }
}


void RubyState::teardown() {
    if(rb_respond_to(_rubyObject, TeardownMethod)) {
        rb_funcall(_rubyObject, TeardownMethod, 0);
    }
}

#pragma mark Event Handlers

void RubyState::keyTyped(KeyEvent *event) {

    if(rb_respond_to(_rubyObject, KeyTypedMethod)) {
        rb_funcall(_rubyObject, KeyTypedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
    }
}

void RubyState::keyPressed(KeyEvent *event) {
    if(rb_respond_to(_rubyObject, KeyPressedMethod)) {
        rb_funcall(_rubyObject, KeyPressedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
    }
}

void RubyState::keyReleased(KeyEvent *event) {
    if(rb_respond_to(_rubyObject, KeyReleasedMethod)) {
        rb_funcall(_rubyObject, KeyReleasedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
    }
}

void RubyState::mouseMoved(MouseMotionEvent *event) {
    if(rb_respond_to(_rubyObject, MouseMovedMethod)) {
//        Info("Here we call mouse_moved on the Ruby state object.");
        rb_funcall(_rubyObject, MouseMovedMethod, 4, INT2NUM(event->absX()), INT2NUM(event->absY()), INT2NUM(event->relX()), INT2NUM(event->relY()));
//        Info("We never get here if MouseMovedMethod = rb_intern(mouse_moved).");
    }
}

void RubyState::mouseClicked(MouseButtonEvent *event) {
    if(rb_respond_to(_rubyObject, MouseClickedMethod)) {
        rb_funcall(_rubyObject, MouseClickedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
    }
}

void RubyState::mousePressed(MouseButtonEvent *event) {
    if(rb_respond_to(_rubyObject, MousePressedMethod)) {
        rb_funcall(_rubyObject, MousePressedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
    }
}

void RubyState::mouseReleased(MouseButtonEvent *event) {
    if(rb_respond_to(_rubyObject, MouseReleasedMethod)) {
        rb_funcall(_rubyObject, MouseReleasedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
    }
}
