#include "MHLoadingState.h"
#include "MHCore.h"

#include <Engine/Window.h>

#pragma mark MHLoadingState ruby bindings
void MHLoadingState::SetupBindings() {
    Class = rb_define_class("MHLoadingState", Class);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHLoadingState::Initialize), 0);
	rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(MHLoadingState::Teardown), 0);
	rb_define_method(Class, "manager=", RUBY_METHOD_FUNC(MHLoadingState::SetManager), 1);
	rb_define_method(Class, "manager", RUBY_METHOD_FUNC(MHLoadingState::GetManager), 0);
}

VALUE MHLoadingState::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new MHLoadingState(self));
    return self;
}

VALUE MHLoadingState::Teardown(VALUE self) {
    Info("Tearing down LoadingState");
	return self;
}

VALUE MHLoadingState::SetManager(VALUE self, VALUE manager) {
    MHLoadingState *state = (MHLoadingState*)GetObject(self);

    if (NIL_P(manager)) {
        // Delete the manager if given nil.
        delete state->_manager;
        state->_manager = NULL;
    } else if (state->_manager) {
        // This probably isn't what people someone wanted to do.
        THROW(InvalidStateError, "Memory leak!");
    } else {
        // The typical case. Set the world in the proper game state object.
        state->_manager = MHUIManager::GetObject(manager);
    }

    return rb_iv_set(self, "@manager", manager);
}

VALUE MHLoadingState::GetManager(VALUE self) {
    MHLoadingState *state = (MHLoadingState*)GetObject(self);
    return MHUIManager::GetValue(state->_manager);
}


#pragma mark MHLoadingState declarations
MHLoadingState::MHLoadingState(VALUE robj): RubyStateProxy(robj), _manager(NULL) {
	MHCore::GetWindow()->setBGColor(Color4(.6,.6,.6,1));
    MHCore::GetWindow()->addViewport(NULL, 0, 0.0f, 0.0f, 1.0f, 1.0f);
}

MHLoadingState::~MHLoadingState() {}
