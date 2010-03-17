#include "MHUIElement.h"
#include <Render/MaterialManager.h>

MHUIElement::MHUIElement(const std::string name, MHUIManager *manager, const std::string mat): Entity(NULL), _manager(manager) {
	_manager->addElement(name, this);
	setMaterial(MaterialManager::Get()->loadResource(mat));
}

MHUIElement::~MHUIElement() {}

void MHUIElement::render(RenderContext* context) {
	Info("Rendering UIElement");
	context->setActiveMaterial(getMaterial());
    /*glBegin(GL_QUADS);
        glVertex3f(_position[0],        _position[1],         _position[2]);
        glVertex3f(_position[0]+_width, _position[1],         _position[2]);
		glVertex3f(_position[0]+_width, _position[1]+_height, _position[2]);
        glVertex3f(_position[0],        _position[1]+_height, _position[2]);
		
		glVertex3f(_position[0],        _position[1], _position[2]);
        glVertex3f(_position[0]+_width, _position[1], _position[2]);
		glVertex3f(_position[0]+_width, _position[1], _position[2]+_height);
        glVertex3f(_position[0],        _position[1], _position[2]+_height);
    glEnd();*/
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(0.5f, 0.0f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(0.0f, 0.5f);
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
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 3);
    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElement::SetDimensions), 4);
}

VALUE MHUIElement::Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat) {
    std::string strName = rb_string_value_cstr(&name);
    std::string strMat = rb_string_value_cstr(&mat);

    MHUIManager *objManager = MHUIManager::GetObject(manager);

    MHUIElement::RegisterObject(self, new MHUIElement(strName, objManager, strMat));
    return self;
}

VALUE MHUIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    thisElement->_width = NUM2INT(w);
    thisElement->_height = NUM2INT(h);
    return self;
}

VALUE MHUIElement::AddChild(VALUE self, VALUE child) {
    MHUIElement *pElement = GetObject(child);
    GetObject(self)->_children.push_back(pElement);
    return self;
};
