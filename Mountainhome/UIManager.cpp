#include "UIManager.h"

UIManager::UIManager() {}

UIManager::~UIManager() {}

void UIManager::SetupBindings() {
    Class = rb_define_class("UIManager", rb_cObject);
}

void UIManager::render(RenderContext* context) {}
