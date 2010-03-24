#include "MHUIElement.h"
#include <Render/MaterialManager.h>
#include <Render/FontManager.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHUIElement::SetupBindings() {
    Class = rb_define_class("MHUIElement", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 4);
    rb_define_method(Class, "cull_child", RUBY_METHOD_FUNC(MHUIElement::CullChild), 1);
    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElement::SetDimensions), 4);
    rb_define_method(Class, "set_offset", RUBY_METHOD_FUNC(MHUIElement::SetOffset), 2);
    rb_define_method(Class, "set_border", RUBY_METHOD_FUNC(MHUIElement::SetBorder), 1);
    rb_define_method(Class, "add_child", RUBY_METHOD_FUNC(MHUIElement::AddChild), 1);
    rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(MHUIElement::SetPosition), 2);
    rb_define_method(Class, "move_relative", RUBY_METHOD_FUNC(MHUIElement::MoveRelative), 2);
    rb_define_method(Class, "resize", RUBY_METHOD_FUNC(MHUIElement::Resize), 2);
    rb_define_method(Class, "always_on_top", RUBY_METHOD_FUNC(MHUIElement::AlwaysOnTop), 0);
    rb_define_method(Class, "text=", RUBY_METHOD_FUNC(MHUIElement::SetText), 1);
    rb_define_method(Class, "text", RUBY_METHOD_FUNC(MHUIElement::GetText), 0);
    rb_define_method(Class, "x=", RUBY_METHOD_FUNC(MHUIElement::XEquals), 1);
    rb_define_method(Class, "y=", RUBY_METHOD_FUNC(MHUIElement::YEquals), 1);
    rb_define_method(Class, "x", RUBY_METHOD_FUNC(MHUIElement::X), 0);
    rb_define_method(Class, "y", RUBY_METHOD_FUNC(MHUIElement::Y), 0);
    rb_define_method(Class, "w", RUBY_METHOD_FUNC(MHUIElement::W), 0);
    rb_define_method(Class, "h", RUBY_METHOD_FUNC(MHUIElement::H), 0);
}

VALUE MHUIElement::Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat, VALUE text) {
    std::string strName = rb_string_value_cstr(&name);
    std::string strMat = rb_string_value_cstr(&mat);
    std::string strText = rb_string_value_cstr(&text);

    MHUIManager *objManager = MHUIManager::GetObject(manager);

    MHUIElement::RegisterObject(self, new MHUIElement(strName, objManager, strMat, strText));
    return self;
}

VALUE MHUIElement::CullChild(VALUE self, VALUE child) {
    MHUIElement *tElement = MHUIElement::GetObject(self),
                *pChild   = MHUIElement::GetObject(child);

    tElement->cullChild(pChild);

    return self;
}

VALUE MHUIElement::SetText(VALUE self, VALUE text) {
    std::string strText = rb_string_value_cstr(&text);
    GetObject(self)->_text = strText;

    return self;
}

VALUE MHUIElement::GetText(VALUE self) {
    return rb_str_new2(GetObject(self)->_text.data());
}

VALUE MHUIElement::XEquals(VALUE self, VALUE value) {
    GetObject(self)->_position[0] = NUM2INT(value);
    return value;
}

VALUE MHUIElement::YEquals(VALUE self, VALUE value) {
    GetObject(self)->_position[1] = NUM2INT(value);
    return value;
}

VALUE MHUIElement::X(VALUE self) {
    return INT2NUM(GetObject(self)->_position[0]);
}

VALUE MHUIElement::Y(VALUE self) {
    return INT2NUM(GetObject(self)->_position[1]);
}

VALUE MHUIElement::W(VALUE self) {
    return INT2NUM(GetObject(self)->_width);
}

VALUE MHUIElement::H(VALUE self) {
    return INT2NUM(GetObject(self)->_height);
}

VALUE MHUIElement::SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    thisElement->_width = NUM2INT(w);
    thisElement->_height = NUM2INT(h);
    return self;
}

VALUE MHUIElement::SetOffset(VALUE self, VALUE x, VALUE y) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->_xoffset = NUM2INT(x);
    thisElement->_yoffset = NUM2INT(y);
    return self;
}

VALUE MHUIElement::SetBorder(VALUE self, VALUE border) {
    MHUIElement::GetObject(self)->_border = NUM2INT(border);
    return self;
}

VALUE MHUIElement::SetPosition(VALUE self, VALUE x, VALUE y) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    return self;
}

VALUE MHUIElement::MoveRelative(VALUE self, VALUE x, VALUE y) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->moveRelative(Vector3(NUM2INT(x), NUM2INT(y), 0.0));
    return self;
}

VALUE MHUIElement::Resize(VALUE self, VALUE x, VALUE y) {
    MHUIElement *thisElement = GetObject(self);
    thisElement->_width = NUM2INT(x);
    thisElement->_height = NUM2INT(y);
    return self;
}

VALUE MHUIElement::AlwaysOnTop(VALUE self) {
    MHUIElement::GetObject(self)->_onTop = true;
    return self;
}

VALUE MHUIElement::AddChild(VALUE self, VALUE child) {
    MHUIElement *pElement = GetObject(child);
    GetObject(self)->_children.push_back(pElement);
    return self;
};

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHUIElement::MHUIElement(const std::string name, MHUIManager *manager, const std::string mat, const std::string text): 
    Entity(NULL), _manager(manager), _text(text), _name(name), _xoffset(0), _yoffset(0), _onTop(false), _border(0) {
    if(mat.length()>0) {
        setMaterial(MaterialManager::Get()->getOrLoadResource(mat));
    }
}

MHUIElement::~MHUIElement() {
    MHUIElement::UnregisterObject(this);
    clearChildren();
}

void MHUIElement::clearChildren() {
    clear_list(_children);
}

void MHUIElement::cullChild(MHUIElement *child) {
    if(_children.empty()) { 
        Info("No children to cull!");
        return; 
    }

    std::list<MHUIElement*>::iterator it;
    for(it = _children.begin(); it != _children.end(); it++) {
        if ((*it) == child) {
            delete (*it);
            _children.erase(it);
            return;
        }
    }
    
    Info("Child to cull not found!");
    return;
}

std::list<MHUIElement*> MHUIElement::enqueue() {
    std::list<MHUIElement*> deferred;

    if(!_onTop) {
        RenderQueue::Get()->addEntity(this);
    } else {
        deferred.push_back(this);
    }

    std::list<MHUIElement*>::iterator it;
    for(it=_children.begin(); it!=_children.end(); it++) {
        std::list<MHUIElement*> retElems;
        retElems = (*it)->enqueue();
        deferred.insert(deferred.end(), retElems.begin(), retElems.end());
    }

    return deferred;
}

void MHUIElement::render(RenderContext* context) {
    glPushMatrix();
    glTranslatef(_position[0], _position[1], _position[2]);

    if (getMaterial()) { 
        int x1 = _xoffset,
            x2 = _xoffset + _width,
            y1 = _yoffset,
            y2 = _yoffset + _height;

        if(_border > 0) {
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
                glVertex2f(x1-_border, y1-_border);
                glVertex2i(x2+_border, y1-_border);
                glVertex2i(x2+_border, y2+_border);
                glVertex2i(x1-_border, y2+_border);
            glEnd();
        }

        context->setActiveMaterial(getMaterial());
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(x1, y1);
            glTexCoord2f(1, 0); glVertex2i(x2, y1);
            glTexCoord2f(1, 1); glVertex2i(x2, y2);
            glTexCoord2f(0, 1); glVertex2i(x1, y2);
        glEnd();
    }
    
    if(_text.length()>0) {
        Font *font = _manager->getFont();
        font->setColor(0.0f, 0.0f, 0.0f, 1.0f);
        font->print(_position[0],_position[1],_text.data());
    }  

    glPopMatrix();
}
