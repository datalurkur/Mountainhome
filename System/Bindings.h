/*
 *  Bindings.h
 *  Mountainhome
 *
 *  Created by Brent Wilson on 2/13/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _BINDINGS_H_
#define _BINDINGS_H_
// Wtf hack....
#define HAVE_STRUCT_TIMESPEC 1
#include <ruby.h>

#include <Engine/ParentState.h>

class MountainhomeBinding {
public:
    static void Setup();
    static VALUE StopMainLoop(VALUE self);
    static VALUE RegisterState(VALUE self, VALUE state, VALUE name);
    static VALUE SetState(VALUE self, VALUE name);

protected:
    static VALUE RubyObject;
    static VALUE RubyClass;

};

class StateBinding : public ParentState {
public:
    static void Setup();
    static StateBinding* GetState(VALUE robj);

    static VALUE Initialize(VALUE self);
    static VALUE RegisterState(VALUE self);

    typedef std::map<VALUE, StateBinding*> RubyObjectMap;

protected:
    static RubyObjectMap RubyObjects;
    static VALUE UpdateMethod;
    static VALUE RubyClass;

public:
    StateBinding(VALUE robj);
    virtual ~StateBinding();
    virtual void update();

private:
    VALUE _rubyObject;

};

#endif
