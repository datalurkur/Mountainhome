#include "Bindings.h"

BoundState::BoundState() {
}

BoundState::~BoundState() {
}

void BoundState::update() { 
  TRACE_FUNCTION_ENTRY(); 
}

VALUE RubyState::Class;

void RubyState::Setup() {
  TRACE_FUNCTION_ENTRY();
  RubyState::Class = rb_define_class("State", rb_cObject);
  rb_define_method(RubyState::Class, "initialize", RUBY_METHOD_FUNC(RubyState_init), 0);
}

RubyState::RubyState(VALUE obj): RObj(obj) { 
  TRACE_FUNCTION_ENTRY(); 
}

void RubyState::update() {
  TRACE_FUNCTION_ENTRY();
  rb_funcall(RObj, rb_intern("update"), 0);
}

VALUE RubyState_init(VALUE self) {
    TRACE_FUNCTION_ENTRY();
    rb_iv_set(self, "@c_obj", (VALUE)new RubyState(self));
    return self;
}

///////////////////////////////////////////////////////////////////////////////

Engine::Engine(): _running(true), _state(NULL) {
}

void Engine::setState(BoundState *state) { 
  TRACE_FUNCTION_ENTRY(); _state = state;
}

void Engine::startMainLoop() { 
  TRACE_FUNCTION_ENTRY(); 
  while(_running) {
    _state->update();
  }
}

void Engine::exitMainLoop() {
  TRACE_FUNCTION_ENTRY(); _running = false;
}

VALUE RubyEngine::Class;

void RubyEngine::Setup() {
  TRACE_FUNCTION_ENTRY();
  RubyEngine::Class = rb_define_class("Engine", rb_cObject);
  rb_define_method(RubyEngine::Class, "state=", RUBY_METHOD_FUNC(Engine_setState), 1);
  rb_define_method(RubyEngine::Class, "exit", RUBY_METHOD_FUNC(Engine_exitMainLoop), 0);
}

RubyEngine::RubyEngine() {
  TRACE_FUNCTION_ENTRY();
  RObj = rb_funcall(RubyEngine::Class, rb_intern("new"), 0);
  rb_iv_set(RObj, "@c_obj", (VALUE)this);
}

VALUE Engine_setState(VALUE self, VALUE state) {
    TRACE_FUNCTION_ENTRY();
    Engine *e = (Engine*)rb_iv_get(self, "@c_obj");
    BoundState *s = (BoundState*)rb_iv_get(state, "@c_obj");
    e->setState(s);
    return state;
}

VALUE Engine_exitMainLoop(VALUE self) {
    TRACE_FUNCTION_ENTRY();
    Engine *e = (Engine*)rb_iv_get(self, "@c_obj");
    e->exitMainLoop();
    return self;
}
