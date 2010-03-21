#include "MHUIElement.h"
#include <Render/MaterialManager.h>
#include <Render/FontManager.h>

MHUIElement::MHUIElement(const std::string name, MHUIManager *manager, const std::string mat, const std::string text): Entity(NULL), _manager(manager) {
    _text = text;
	_manager->addElement(name, this);
	setMaterial(MaterialManager::Get()->getOrLoadResource(mat));
}

MHUIElement::~MHUIElement() {}

void MHUIElement::render(RenderContext* context) {
    // Eventually, render the text
    _manager->getFont()->print("Hello World", 10, 10);
	
	if (getMaterial()) { context->setActiveMaterial(getMaterial()); }
    glBegin(GL_QUADS);
        glVertex2f(_position[0],          _position[1]          ); glTexCoord2f(0, 0);
        glVertex2f(_position[0],          _position[1] - _height); glTexCoord2f(1, 0);
		glVertex2f(_position[0] + _width, _position[1] - _height); glTexCoord2f(1, 1);
        glVertex2f(_position[0] + _width, _position[1]          ); glTexCoord2f(0, 1);
    glEnd();

	/* TODO: Setup positional children by modifying the context prior to them rendering. */

    /* Loop through the children, drawing them. */
    std::list<MHUIElement*>::iterator it;
    for (it=_children.begin() ; it != _children.end(); it++) {
        (*it)->render(context);
    }
}

void MHUIElement::SetupBindings() {
    Class = rb_define_class("MHUIElement", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 4);
    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElement::SetDimensions), 4);
    rb_define_method(Class, "add_child", RUBY_METHOD_FUNC(MHUIElement::AddChild), 1);
    rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(MHUIElement::SetPosition), 2);
    rb_define_method(Class, "x=", RUBY_METHOD_FUNC(MHUIElement::XEquals), 1);
    rb_define_method(Class, "y=", RUBY_METHOD_FUNC(MHUIElement::YEquals), 1);
    rb_define_method(Class, "x", RUBY_METHOD_FUNC(MHUIElement::X), 0);
    rb_define_method(Class, "y", RUBY_METHOD_FUNC(MHUIElement::Y), 0);
}

VALUE MHUIElement::Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat, VALUE text) {
    std::string strName = rb_string_value_cstr(&name);
    std::string strMat = rb_string_value_cstr(&mat);
    std::string strText = rb_string_value_cstr(&text);

    MHUIManager *objManager = MHUIManager::GetObject(manager);

    MHUIElement::RegisterObject(self, new MHUIElement(strName, objManager, strMat, strText));
    return self;
}

VALUE MHUIElement::XEquals(VALUE self, VALUE value) {
    GetObject(self)->_position[0] = NUM2DBL(value);
    return value;
}

VALUE MHUIElement::YEquals(VALUE self, VALUE value) {
    GetObject(self)->_position[1] = NUM2DBL(value);
    return value;
}

VALUE MHUIElement::X(VALUE self) {
    return DBL2NUM(GetObject(self)->_position[0]);
}

VALUE MHUIElement::Y(VALUE self) {
    return DBL2NUM(GetObject(self)->_position[1]);
}

VALUE MHUIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2DBL(x), NUM2DBL(y), 0.0);
    thisElement->_width = NUM2DBL(w);
    thisElement->_height = NUM2DBL(h);
	return self;
}

VALUE MHUIElement::SetPosition(VALUE self, VALUE x, VALUE y) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2DBL(x), NUM2DBL(y), 0.0);
	return self;
}

VALUE MHUIElement::AddChild(VALUE self, VALUE child) {
    MHUIElement *pElement = GetObject(child);
    GetObject(self)->_children.push_back(pElement);
    return self;
};
