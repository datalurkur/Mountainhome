#include "MHUIManager.h"

MHUIManager::MHUIManager(const std::string looknfeel) {
    // Set up the looknfeel
}

MHUIManager::~MHUIManager() {}

void MHUIManager::SetupBindings() {
    Class = rb_define_class("MHUIManager", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIManager::Initialize), 1);
}

VALUE MHUIManager::Initialize(VALUE self, VALUE looknfeel) {
    std::string strLooknfeel = rb_string_value_cstr(&looknfeel);
    RegisterObject(self, new MHUIManager(strLooknfeel));
    return self;
}

void MHUIManager::render(RenderContext* context) {}
void MHUIManager::resize(int width, int height) {}
