#include "MHUIManager.h"

MHUIManager::MHUIManager() {}

MHUIManager::~MHUIManager() {}

void MHUIManager::SetupBindings() {
    Class = rb_define_class("MHUIManager", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 0);
}

VALUE MHUIManager::Initialize(VALUE self) {
    return self;
}

void MHUIManager::render(RenderContext* context) {}
