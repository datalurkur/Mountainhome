#include "MHUIElement.h"

void MHUIElement::SetupBindings() {
    Class = rb_define_class("MHUIElement", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 4);
    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElement::SetDimensions), 4);
}

VALUE MHUIElement::Initialize(VALUE self, VALUE name, VALUE manager, VALUE model, VALUE mat) {
    std::string strName = rb_string_value_cstr(&name);
    std::string strModel = rb_string_value_cstr(&model);
    std::string strMat = rb_string_value_cstr(&mat);

    MHUIManager *objManager = MHUIManager::GetObject(manager);

	// This needs to be fixed to first load a model, then call the Entity constructor
    //MHUIElement::RegisterObject(self, new MHUIElement(strName, objManager, strModel, strMat));
    return self;
}

VALUE MHUIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    thisElement->_width = w;
    thisElement->_height = h;
	// This needs to update the model in the Entity to account for the new dimensions
    return self;
}
