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
ID RubyState::TeardownMethod = NULL;
ID RubyState::SetupMethod    = NULL;
ID RubyState::UpdateMethod   = NULL;

ID RubyState::ConvertEventMethod  = NULL;

ID RubyState::KeyPressedMethod    = NULL;
ID RubyState::KeyReleasedMethod   = NULL;
ID RubyState::MouseMovedMethod    = NULL;
ID RubyState::MouseClickedMethod  = NULL;
ID RubyState::MousePressedMethod  = NULL;
ID RubyState::MouseReleasedMethod = NULL;

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

    ConvertEventMethod  = rb_intern("convert_event");

    KeyPressedMethod    = rb_intern("key_pressed");
    KeyReleasedMethod   = rb_intern("key_released");
    MouseMovedMethod    = rb_intern("mouse_moved");
    MouseClickedMethod  = rb_intern("mouse_clicked");
    MousePressedMethod  = rb_intern("mouse_pressed");
    MouseReleasedMethod = rb_intern("mouse_released");

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

#define CALL_CONVERT_EVENT(argc, id, ...) \
	VALUE argv[argc] = { ID2SYM(id), __VA_ARGS__ }; \
	if(rb_respond_to(_rubyObject, ConvertEventMethod)) { \
		rb_funcall2(_rubyObject, ConvertEventMethod, argc, argv); \
	}

void RubyState::keyPressed(KeyEvent *event) {
	VALUE argv[3] = { KeyPressedMethod, INT2NUM(event->key()), INT2NUM(event->modifier()) }; \
	if(rb_respond_to(_rubyObject, ConvertEventMethod)) { \
		rb_funcall2(_rubyObject, ConvertEventMethod, 3, argv); \
	}
//	CALL_CONVERT_EVENT(3, KeyPressedMethod, INT2NUM(event->key()), INT2NUM(event->modifier()))
}

void RubyState::keyReleased(KeyEvent *event) {
	int argc = 3;
	VALUE argv[argc];
	argv[0] = ID2SYM(KeyReleasedMethod);
	argv[1] = INT2NUM(event->key());
	argv[2] = INT2NUM(event->modifier());
	
	if(rb_respond_to(_rubyObject, rb_intern("convert_event"))) {
		rb_funcall2(_rubyObject, rb_intern("convert_event"), argc, argv);
	}
}

void RubyState::mouseMoved(MouseMotionEvent *event) {
    if(rb_respond_to(_rubyObject, MouseMovedMethod)) {
        rb_funcall(_rubyObject, MouseMovedMethod, 4, INT2NUM(event->absX()), INT2NUM(event->absY()), INT2NUM(event->relX()), INT2NUM(event->relY()));
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
