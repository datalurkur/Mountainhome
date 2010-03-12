#include "UIElement.h"

void UIElement::SetupBindings() {
    Class = rb_define_class("UIElement", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(UIElement::Initialize), 4);
    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(UIElement::SetDimensions), 4);
}

VALUE UIElement::Initialize(VALUE self, VALUE name, VALUE manager, VALUE model, VALUE mat) {
    std::string strName = rb_string_value_cstr(&name);
    std::string strModel = rb_string_value_cstr(&model);
    std::string strMat = rb_string_value_cstr(&mat);

    UIManager *objManager = UIManager::GetObject(manager);

	// This needs to be fixed to first load a model, then call the Entity constructor
    //UIElement::RegisterObject(self, new UIElement(strName, objManager, strModel, strMat));
    return self;
}

VALUE UIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    UIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    thisElement->_width = w;
    thisElement->_height = h;
	// This needs to update the model in the Entity to account for the new dimensions
    return self;
}