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
// XXXBMW: Wtf hack....
#define HAVE_STRUCT_TIMESPEC 1
#include <ruby.h>

#include <Engine/ParentState.h>

class MountainhomeBinding {
public:
    static void SetupBindings();
    static VALUE StopMainLoop(VALUE self);
    static VALUE RegisterState(VALUE self, VALUE state, VALUE name);
    static VALUE SetState(VALUE self, VALUE name);

protected:
    static VALUE RubyObject;
    static VALUE RubyClass;

};

class StateBinding : public ParentState {
public:
    static void SetupBindings();
    static StateBinding* GetState(VALUE robj);

    static VALUE Setup(VALUE self);
    static VALUE Teardown(VALUE self);
    static VALUE Initialize(VALUE self);
    static VALUE RegisterState(VALUE self);

    typedef std::map<VALUE, StateBinding*> RubyObjectMap;

protected:
    static RubyObjectMap RubyObjects;
    static VALUE TeardownMethod;
    static VALUE SetupMethod;
    static VALUE UpdateMethod;
    static VALUE RubyClass;

public:
    StateBinding(VALUE robj);
    virtual ~StateBinding();
    virtual void update(int elapsed);
    virtual void setup(va_list args);
    virtual void teardown();

private:
    VALUE _rubyObject;

};

#endif
