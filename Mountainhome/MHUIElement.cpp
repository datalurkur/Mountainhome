#include "MHUIElement.h"

MHUIElement::MHUIElement(const std::string name, MHUIManager *manager, const std::string model, const std::string mat): Entity(NULL) {
    
}

MHUIElement::~MHUIElement() {}

void MHUIElement::render(RenderContext* context) {
    glBegin(GL_QUADS);
        glVertex3f(_position[0],        _position[1],         _position[2]);
        glVertex3f(_position[0]+_width, _position[1],         _position[2]);
		glVertex3f(_position[0]+_width, _position[1]+_height, _position[2]);
        glVertex3f(_position[0],        _position[1]+_height, _position[2]);
    glEnd();
}

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

    MHUIElement::RegisterObject(self, new MHUIElement(strName, objManager, strModel, strMat));
    return self;
}

VALUE MHUIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    thisElement->_width = NUM2INT(w);
    thisElement->_height = NUM2INT(h);
    return self;
}

VALUE MHUIElement::SetParent(VALUE self, VALUE parent) {
    MHUIElement *pElement = MHUIElement::GetObject(parent);
    GetObject(self)->_parent = pElement;

    return self;
};
