#include "MHUIElement.h"

#include <Render/MaterialManager.h>
#include <Render/FontManager.h>
#include <Render/Font.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHUIElement::SetupBindings() {
    Class = rb_define_class("MHUIElement", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIElement::Initialize), 5);
    rb_define_method(Class, "add_child", RUBY_METHOD_FUNC(MHUIElement::AddChild), 1);
    rb_define_method(Class, "cull_child", RUBY_METHOD_FUNC(MHUIElement::CullChild), 1);
    rb_define_method(Class, "cull_children", RUBY_METHOD_FUNC(MHUIElement::CullChildren), 0);
    rb_define_method(Class, "each_child", RUBY_METHOD_FUNC(MHUIElement::EachChild), 0);

    rb_define_method(Class, "set_dimensions", RUBY_METHOD_FUNC(MHUIElement::SetDimensions), 4);
    rb_define_method(Class, "set_offset", RUBY_METHOD_FUNC(MHUIElement::SetOffset), 2);
    rb_define_method(Class, "set_border", RUBY_METHOD_FUNC(MHUIElement::SetBorder), 1);
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
    rb_define_method(Class, "x_offset", RUBY_METHOD_FUNC(MHUIElement::XOffset), 0);
    rb_define_method(Class, "y_offset", RUBY_METHOD_FUNC(MHUIElement::YOffset), 0);
    rb_define_alloc_func(Class, MHUIElement::Alloc);
}

void MHUIElement::Mark(MHUIElement *cSelf) {
    std::list<MHUIElement*>::iterator itr = cSelf->_children.begin();
    for (; itr != cSelf->_children.end(); itr++) {
        rb_gc_mark(MHUIElement::GetValue(*itr));
    }
}

VALUE MHUIElement::Initialize(VALUE rSelf, VALUE rName, VALUE rManager, VALUE rMatName, VALUE rFontName, VALUE rText) {
    AssignCObjFromValue(MHUIManager, cManager, rManager);
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    std::string cName     = rb_string_value_cstr(&rName);
    std::string cMatName  = rb_string_value_cstr(&rMatName);
    std::string cFontName = rb_string_value_cstr(&rFontName);
    std::string cText     = rb_string_value_cstr(&rText);

    Material *cMat = NULL;
    if (cMatName.size() > 0) {
        cMat = cManager->getMaterialManager()->getOrLoadResource(cMatName);
    }

    Font *cFont = cManager->getFont();
    if (cFontName.size() > 0) {
        cFont = cManager->getFontManager()->getOrLoadResource(cFontName);
    }

    cSelf->initialize(cName, cManager, cMat, cFont, cText);
    
    return rSelf;
}

#pragma mark Child Functions

VALUE MHUIElement::AddChild(VALUE rSelf, VALUE rChild) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    AssignCObjFromValue(MHUIElement, cChild, rChild);
    cSelf->_children.push_back(cChild);
    return rSelf;
}

VALUE MHUIElement::CullChild(VALUE rSelf, VALUE rChild) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    AssignCObjFromValue(MHUIElement, cChild, rChild);
    cSelf->cullChild(cChild);
    return rSelf;
}

VALUE MHUIElement::CullChildren(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->cullChildren();
    return rSelf;
}

VALUE MHUIElement::EachChild(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    std::list<MHUIElement*>::iterator itr = cSelf->_children.begin();
    for (; itr != cSelf->_children.end(); itr++) {
        rb_yield(MHUIElement::GetValue(*itr));
    }
    return rSelf;
}

#pragma mark Text Functions

VALUE MHUIElement::SetText(VALUE rSelf, VALUE rText) {
    std::string cText = rb_string_value_cstr(&rText);
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_text = cText;
    return rSelf;
}

VALUE MHUIElement::GetText(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return rb_str_new2(cSelf->_text.c_str());
}

#pragma mark Position Functions

VALUE MHUIElement::XEquals(VALUE rSelf, VALUE value) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_position[0] = NUM2INT(value);
    return value;
}

VALUE MHUIElement::YEquals(VALUE rSelf, VALUE value) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_position[1] = NUM2INT(value);
    return value;
}

VALUE MHUIElement::X(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_position.x);
}

VALUE MHUIElement::Y(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_position.y);
}

VALUE MHUIElement::W(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_width);
}

VALUE MHUIElement::H(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_height);
}

VALUE MHUIElement::XOffset(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_xoffset);
}

VALUE MHUIElement::YOffset(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    return INT2NUM(cSelf->_yoffset);
}

VALUE MHUIElement::SetDimensions(VALUE rSelf, VALUE x, VALUE y, VALUE w, VALUE h) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    cSelf->_width = NUM2INT(w);
    cSelf->_height = NUM2INT(h);
    return rSelf;
}

VALUE MHUIElement::SetOffset(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_xoffset = NUM2INT(x);
    cSelf->_yoffset = NUM2INT(y);
    return rSelf;
}

VALUE MHUIElement::SetBorder(VALUE rSelf, VALUE border) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_border = NUM2INT(border);
    return rSelf;
}

VALUE MHUIElement::SetPosition(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->setPosition(NUM2INT(x), NUM2INT(y), 0.0);
    return rSelf;
}

VALUE MHUIElement::MoveRelative(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->moveRelative(Vector3(NUM2INT(x), NUM2INT(y), 0.0));
    return rSelf;
}

VALUE MHUIElement::Resize(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_width = NUM2INT(x);
    cSelf->_height = NUM2INT(y);
    return rSelf;
}

VALUE MHUIElement::AlwaysOnTop(VALUE rSelf) {
    AssignCObjFromValue(MHUIElement, cSelf, rSelf);
    cSelf->_onTop = true;
    return rSelf;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHUIElement::MHUIElement(): Entity(NULL), _manager(NULL), _font(NULL), _text(""), _name(""),
_xoffset(0), _yoffset(0), _width(0), _height(0), _onTop(false), _border(0) {}

MHUIElement::~MHUIElement() {
    cullChildren();
}

void MHUIElement::initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font, const std::string &text) {
    _manager = manager;
    _font = font;
    _name = name;
    _text = text;

    setMaterial(mat);
}

bool MHUIElement::cullChild(MHUIElement *child) {
    if(_children.empty()) {
        return false;
    }

    std::list<MHUIElement*>::iterator itr = _children.begin();
    std::list<MHUIElement*>::iterator del = _children.end();
    for(; itr != _children.end(); itr++) {
        if ((*itr) == child) {
            del = itr;
            break;
        }
        else if((*itr)->cullChild(child)) {
            break;
        }
    }

    if (del != _children.end()) {
        _children.erase(del);
        return true;
    }
    else {
        return false;
    }
}

void MHUIElement::cullChildren() {
    std::list<MHUIElement*>::iterator itr = _children.begin();
    for(; itr != _children.end(); itr++) {
        (*itr)->cullChildren();
    }

    _children.clear();
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
                // Bottom border
                glVertex2i(x1-_border, y1-_border);
                glVertex2i(x2+_border, y1-_border);
                glVertex2i(x2+_border, y1        );
                glVertex2i(x1-_border, y1        );

                // Top border
                glVertex2i(x1-_border, y2        );
                glVertex2i(x2+_border, y2        );
                glVertex2i(x2+_border, y2+_border);
                glVertex2i(x1-_border, y2+_border);

                // Left border
                glVertex2i(x1-_border, y1);
                glVertex2i(x1        , y1);
                glVertex2i(x1        , y2);
                glVertex2i(x1-_border, y2);

                // Right border
                glVertex2i(x2        , y1);
                glVertex2i(x2+_border, y1);
                glVertex2i(x2+_border, y2);
                glVertex2i(x2        , y2);
            glEnd();
        }

        context->setActiveMaterial(getMaterial());
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(x1, y1);
            glTexCoord2f(1, 0); glVertex2i(x2, y1);
            glTexCoord2f(1, 1); glVertex2i(x2, y2);
            glTexCoord2f(0, 1); glVertex2i(x1, y2);
        glEnd();
        context->unsetActiveMaterial(getMaterial());
    }
    
    if(_text.length()>0) {
        _font->print(_position[0], _position[1], _manager->getWidth(), _manager->getHeight(), _text.data());
    }  

    glPopMatrix();
}
