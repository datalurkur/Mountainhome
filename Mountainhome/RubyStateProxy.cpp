/*
 *  RubyStateProxy.cpp
 *  System
 *
 *  Created by loch on 2/18/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyStateProxy.h"
#include <Base/Logger.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE RubyStateProxy::TeardownMethod = NULL;
VALUE RubyStateProxy::SetupMethod    = NULL;
VALUE RubyStateProxy::UpdateMethod   = NULL;

VALUE RubyStateProxy::KeyTypedMethod      = NULL;
VALUE RubyStateProxy::KeyPressedMethod    = NULL;
VALUE RubyStateProxy::KeyReleasedMethod   = NULL;
VALUE RubyStateProxy::MouseMovedMethod    = NULL;
VALUE RubyStateProxy::MouseClickedMethod  = NULL;
VALUE RubyStateProxy::MousePressedMethod  = NULL;
VALUE RubyStateProxy::MouseReleasedMethod = NULL;

VALUE RubyStateProxy::Setup(VALUE self) { Info("RSPSetup!"); return self; }
VALUE RubyStateProxy::Teardown(VALUE self) { return self; }
VALUE RubyStateProxy::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new RubyStateProxy(self));
    return self;
}

void RubyStateProxy::SetupBindings() {
    TeardownMethod = rb_intern("teardown");
    UpdateMethod   = rb_intern("update");
    SetupMethod    = rb_intern("setup");
    
    KeyTypedMethod      = rb_intern("key_typed");
    KeyPressedMethod    = rb_intern("key_pressed");
    KeyReleasedMethod   = rb_intern("key_released");
    MouseMovedMethod    = rb_intern("mouse_moved");
    MouseClickedMethod  = rb_intern("mouse_clicked");
    MousePressedMethod  = rb_intern("mouse_pressed");
    MouseReleasedMethod = rb_intern("mouse_released");


    Class = rb_define_class("State", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyStateProxy::Initialize), 0);
    rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(RubyStateProxy::Teardown), 0);
    rb_define_method(Class, "setup", RUBY_METHOD_FUNC(RubyStateProxy::Setup), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
RubyStateProxy::RubyStateProxy(VALUE robj): _rubyObject(robj) {}
RubyStateProxy::~RubyStateProxy() {}

void RubyStateProxy::update(int elapsed) {
    rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed));
}

void RubyStateProxy::setup(va_list args) {
    //\todo This needs to read out the argument list and pass it down to ruby.
    rb_funcall(_rubyObject, SetupMethod, 0);
}


void RubyStateProxy::teardown() {
    rb_funcall(_rubyObject, TeardownMethod, 0);
}

#pragma mark Event Handlers

/*! Delegates to the currently active state.
 * \seealso State::keyTyped */
void RubyStateProxy::keyTyped(KeyEvent *event) {
    if(!rb_respond_to(_rubyObject, KeyTypedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, KeyTypedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
}

/*! Delegates to the currently active state.
 * \seealso State::keyPressed */
void RubyStateProxy::keyPressed(KeyEvent *event) {
    if(!rb_respond_to(_rubyObject, KeyPressedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, KeyPressedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
}

/*! Delegates to the currently active state.
 * \seealso State::keyReleased */
void RubyStateProxy::keyReleased(KeyEvent *event) {
    if(!rb_respond_to(_rubyObject, KeyReleasedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, KeyReleasedMethod, 2, INT2NUM(event->key()), INT2NUM(event->modifier()));
}

/*! Delegates to the currently active state.
 * \seealso State::mouseMoved */
void RubyStateProxy::mouseMoved(MouseMotionEvent *event) {
    if(!rb_respond_to(_rubyObject, MouseMovedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, MouseMovedMethod, 4, INT2NUM(event->absX()), INT2NUM(event->absY()),
        INT2NUM(event->relX()), INT2NUM(event->relY()));
}


/*! Delegates to the currently active state.
 * \seealso State::mouseClicked */
void RubyStateProxy::mouseClicked(MouseButtonEvent *event) {
    if(!rb_respond_to(_rubyObject, MouseClickedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, MouseClickedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
}

/*! Delegates to the currently active state.
 * \seealso State::mousePressed */
void RubyStateProxy::mousePressed(MouseButtonEvent *event) {
    if(!rb_respond_to(_rubyObject, MousePressedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, MousePressedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
}

/*! Delegates to the currently active state.
 * \seealso State::mouseReleased */
void RubyStateProxy::mouseReleased(MouseButtonEvent *event) {
    if(!rb_respond_to(_rubyObject, MouseReleasedMethod)) {
        return;
    }
    rb_funcall(_rubyObject, MouseReleasedMethod, 3, INT2NUM(event->button()), INT2NUM(event->x()), INT2NUM(event->y()));
}