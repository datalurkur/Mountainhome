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
#include "RubyStateBindings.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
ID RubyState::TeardownMethod = NULL;
ID RubyState::SetupMethod    = NULL;
ID RubyState::UpdateMethod   = NULL;
ID RubyState::DrawMethod     = NULL;
ID RubyState::ReceiveEventMethod  = NULL;

RubyState::RubyState(): _rubyObject(0) {
    TeardownMethod  = rb_intern("teardown");
    UpdateMethod    = rb_intern("update");
    DrawMethod      = rb_intern("draw");
    SetupMethod     = rb_intern("setup");
    ReceiveEventMethod = rb_intern("receive_event");
}

RubyState::~RubyState() {}

void RubyState::draw() {
    if(rb_respond_to(_rubyObject, DrawMethod)) {
        rb_funcall(_rubyObject, DrawMethod, 0);
    }
}

void RubyState::update(int elapsed) {
    if(rb_respond_to(_rubyObject, UpdateMethod)) {
        rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed));
    }
}

void RubyState::setup(va_list cArgs) {
    if(rb_respond_to(_rubyObject, SetupMethod)) {
        int argc;
        VALUE *argv;

        RubyStateBindings::DecomposeRubyArray(va_arg(cArgs, VALUE), &argc, &argv);
        rb_funcall2(_rubyObject, SetupMethod, argc, argv);
        delete[] argv;
    }
}


void RubyState::teardown() {
    if(rb_respond_to(_rubyObject, TeardownMethod)) {
        rb_funcall(_rubyObject, TeardownMethod, 0);
    }

    // Cleanup everything after teardown happens.
    // XXXBMW: We may want to do this in a more formalized place. Not certain. This works for now, though.
    rb_gc_start();
}

#pragma mark Event Handlers

/*
   event = klass.new(*argv)
   Event.pass_event(event) if respond_to?(:pass_event)
*/
void RubyState::createRubyEvent(int argc, VALUE *argv, const char *klass) {
    VALUE event = rb_class_new_instance(argc, argv, rb_iv_get(rb_cObject, klass));
    VALUE event_class = rb_iv_get(rb_cObject, "Event");

    if(rb_respond_to(event_class, ReceiveEventMethod)) {
        rb_funcall(event_class, ReceiveEventMethod, 1, event);
    }
}

#define KEY_HELPER(event, klass) \
    VALUE *argv = (VALUE*)malloc(2 * sizeof(VALUE)); \
    argv[0] = INT2NUM(event->key()); \
    argv[1] = INT2NUM(event->modifier()); \
    createRubyEvent(2, argv, klass)

void RubyState::keyPressed(KeyEvent *event) {
    KEY_HELPER(event, "KeyPressed");
}

void RubyState::keyReleased(KeyEvent *event) {
    KEY_HELPER(event, "KeyReleased");
}

void RubyState::mouseMoved(MouseMotionEvent *event) {
    VALUE *argv = (VALUE*)malloc(4 * sizeof(VALUE));
    argv[0] = INT2NUM(event->absX());
    argv[1] = INT2NUM(event->absY());
    argv[2] = INT2NUM(event->relX());
    argv[3] = INT2NUM(event->relY());

    createRubyEvent(4, argv, "MouseMoved");
}

#define MOUSE_HELPER(event, klass) \
    VALUE *argv = (VALUE*)malloc(3 * sizeof(VALUE)); \
    argv[0] = INT2NUM(event->button()); \
    argv[1] = INT2NUM(event->x()); \
    argv[2] = INT2NUM(event->y()); \
    createRubyEvent(3, argv, klass)

void RubyState::mouseClicked(MouseButtonEvent *event) {
    MOUSE_HELPER(event, "MouseClicked");
}

void RubyState::mousePressed(MouseButtonEvent *event) {
    MOUSE_HELPER(event, "MousePressed");
}

void RubyState::mouseReleased(MouseButtonEvent *event) {
    MOUSE_HELPER(event, "MouseReleased");
}
