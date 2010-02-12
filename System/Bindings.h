#ifndef _BINDINGS_H_
#define _BINDINGS_H_

#include "../ruby/include/ruby-1.9.1/ruby.h"
#include <stdio.h>

#define TRACE_FUNCTION_ENTRY() printf("C++: Entering %s:%i\n", __PRETTY_FUNCTION__, __LINE__)

////////////////////////////////////////////////////////////////////////////////
#pragma mark State definitions
////////////////////////////////////////////////////////////////////////////////
class BoundState {
public:
    BoundState();
    ~BoundState();
    virtual void update();
};

class RubyState : public BoundState {
public:
    static VALUE Class;
    static void Setup();

public:
    VALUE RObj;
    RubyState(VALUE obj);
    virtual void update();
};

extern VALUE RubyState_init(VALUE self);

////////////////////////////////////////////////////////////////////////////////
#pragma mark Engine definitions
////////////////////////////////////////////////////////////////////////////////
class Engine {
  bool _running;
  BoundState *_state;

public:
  Engine();

  void setState(BoundState *state);
  void startMainLoop();
  void exitMainLoop();
};

class RubyEngine : public Engine {
public:
    static VALUE Class;
    static void Setup();

public:
    VALUE RObj;
    RubyEngine();
};

extern VALUE Engine_setState(VALUE self, VALUE state);
extern VALUE Engine_exitMainLoop(VALUE self);

////////////////////////////////////////////////////////////////////////////////
//#pragma mark Program entry
////////////////////////////////////////////////////////////////////////////////
/*
int main(int argc, char **argv) {
    TRACE_FUNCTION_ENTRY();
    int state;
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();

        // Create our ruby/c cross over classes
        RubyEngine::Setup();
        RubyState::Setup();

        RubyEngine engine;
        rb_gv_set("$engine", engine.RObj);
        rb_require("./main.rb");
        engine.startMainLoop();
        printf("C++: Main loop finished.\n");
    }

    return ruby_cleanup(state);
}
*/

#endif
