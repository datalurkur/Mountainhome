#include <ruby.h>
#include <stdio.h>

#define TRACE_FUNCTION_ENTRY() printf("C++: Entering %s:%i\n", __PRETTY_FUNCTION__, __LINE__)

////////////////////////////////////////////////////////////////////////////////
#pragma mark State definitions
////////////////////////////////////////////////////////////////////////////////
class State {
public:
    State() {}
    ~State() {}
    virtual void update() { TRACE_FUNCTION_ENTRY(); }
};

VALUE RubyState_init(VALUE self);
class RubyState : public State {
public:
    static VALUE Class;
    static void Setup() {
        TRACE_FUNCTION_ENTRY();
        RubyState::Class = rb_define_class("State", rb_cObject);
        rb_define_method(RubyState::Class, "initialize", RUBY_METHOD_FUNC(RubyState_init), 0);
    }

public:
    VALUE RObj;
    RubyState(VALUE obj): RObj(obj) { TRACE_FUNCTION_ENTRY(); }
    virtual void update() {
        TRACE_FUNCTION_ENTRY();
        rb_funcall(RObj, rb_intern("update"), 0);
    }
};

VALUE RubyState::Class;

VALUE RubyState_init(VALUE self) {
    TRACE_FUNCTION_ENTRY();
    rb_iv_set(self, "@c_obj", (VALUE)new RubyState(self));
    return self;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Engine definitions
////////////////////////////////////////////////////////////////////////////////
class Engine {
  bool _running;
  State *_state;

public:
    Engine(): _running(true), _state(NULL) {}

    void setState(State *state) { TRACE_FUNCTION_ENTRY(); _state = state;                        }
    void startMainLoop()        { TRACE_FUNCTION_ENTRY(); while (_running) { _state->update(); } }
    void exitMainLoop()         { TRACE_FUNCTION_ENTRY(); _running = false;                      }
};

VALUE Engine_setState(VALUE self, VALUE state);
VALUE Engine_exitMainLoop(VALUE self);
class RubyEngine : public Engine {
public:
    static VALUE Class;
    static void Setup() {
        TRACE_FUNCTION_ENTRY();
        RubyEngine::Class = rb_define_class("Engine", rb_cObject);
        rb_define_method(RubyEngine::Class, "state=", RUBY_METHOD_FUNC(Engine_setState), 1);
        rb_define_method(RubyEngine::Class, "exit", RUBY_METHOD_FUNC(Engine_exitMainLoop), 0);
    }

public:
    VALUE RObj;

    RubyEngine() {
        TRACE_FUNCTION_ENTRY();
        RObj = rb_funcall(RubyEngine::Class, rb_intern("new"), 0);
        rb_iv_set(RObj, "@c_obj", (VALUE)this);
    }
};

VALUE RubyEngine::Class;

VALUE Engine_setState(VALUE self, VALUE state) {
    TRACE_FUNCTION_ENTRY();
    Engine *e = (Engine*)rb_iv_get(self, "@c_obj");
    State *s = (State*)rb_iv_get(state, "@c_obj");
    e->setState(s);
    return state;
}

VALUE Engine_exitMainLoop(VALUE self) {
    TRACE_FUNCTION_ENTRY();
    Engine *e = (Engine*)rb_iv_get(self, "@c_obj");
    e->exitMainLoop();
    return self;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Program entry
////////////////////////////////////////////////////////////////////////////////
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