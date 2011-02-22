/*
 *  MHUIElement.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/Matrix.h>
#include <Render/Renderable.h>
#include "MHUIElement.h"

MHUIElement::MHUIElement():
    _localPosition(0, 0),
    _absolutePosition(0, 0),
    _onTop(false),
    _parent(NULL)
{}

MHUIElement::~MHUIElement() {
    deleteAllRenderables();
    deleteAllChildren();
}

// Event management.
void MHUIElement::onMouseOver(int x, int y) {}

void MHUIElement::onClick(int x, int y, int button) {}

// Child management.
void MHUIElement::deleteAllChildren() {
    std::list<MHUIElement*>::iterator itr = _children.begin();
    for(; itr != _children.end(); itr++) {
        (*itr)->deleteAllChildren();
        delete (*itr);
    }

    _children.clear();
}

bool MHUIElement::deleteChild(MHUIElement *child) {
    bool deleted = false;
    std::list<MHUIElement*>::iterator itr = _children.begin();
    for(; itr != _children.end() && !deleted; itr++) {
        if ((*itr) == child) {
            _children.erase(itr);
            deleted = true;
            delete *itr;
        } else {
            deleted = (*itr)->deleteChild(child);
        }
    }

    return deleted;
}

void MHUIElement::addChild(MHUIElement* child) {
    _children.push_back(child);
    child->setParent(this);
}

const std::list<MHUIElement *> & MHUIElement::getChildren() {
    return _children;
}

MHUIElement * MHUIElement::getParent() {
    return _parent;
}

// Position management.
int  MHUIElement::getX() { return _localPosition.x; }
int  MHUIElement::getY() { return _localPosition.y; }
bool MHUIElement::getAlwaysOnTop() { return _onTop; }

void MHUIElement::setX(int x) { _localPosition.x = x; }
void MHUIElement::setY(int y) { _localPosition.y = y; }
void MHUIElement::setAlwaysOnTop(bool val) { _onTop = val; }

void MHUIElement::updateDerivedValues() {
    if (_parent) {
        _absolutePosition = getParent()->_absolutePosition + _localPosition;
    } else {
        _absolutePosition = _localPosition;
    }

    // Update the renderable model matrices.
    Matrix absolute = Matrix::Translation(Vector3(
        _absolutePosition.x, _absolutePosition.y, 0));

    RenderableList::iterator rItr = _renderables.begin();
    for (; rItr != _renderables.end(); rItr++) {
        (*rItr)->setModelMatrix(absolute);
    }

    // And update the children.
    std::list<MHUIElement*>::iterator cItr = _children.begin();
    for(; cItr != _children.end(); cItr++) {
        (*cItr)->updateDerivedValues();
    }
}

// Renderable management.
void MHUIElement::deleteAllRenderables() {
    clear_list(_renderables);
}

void MHUIElement::deleteRenderable(Renderable *renderable) {
    RenderableList::iterator itr = _renderables.begin();
    while (itr != _renderables.end()) {
        if ((*itr) == renderable) {
            delete *itr;
            itr = _renderables.erase(itr);
        } else {
            itr++;
        }
    }
}

void MHUIElement::addRenderable(Renderable* renderable) {
    _renderables.push_back(renderable);
}

void MHUIElement::addRenderablesToList(RenderableList &list) {
    RenderableList::iterator rItr = _renderables.begin();
    for (; rItr != _renderables.end(); rItr++) {
        list.push_back(*rItr);
    }

    std::list<MHUIElement*>::iterator cItr = _children.begin();
    for (; cItr != _children.end(); cItr++) {
        (*cItr)->addRenderablesToList(list);
    }
}

void MHUIElement::setParent(MHUIElement *parent) {
    ASSERT(!_parent);
    _parent = parent;
}
















//MHUIElement::MHUIElement(const std::string &name, Material *mat, Font *font);
//MHUIElement::~MHUIElement();
//
//void MHUIElement::preRenderNotice();
//void MHUIElement::postRenderNotice();
//
//
//
//
//int MHUIElement::getTextWidth() {
//    return _font->getWidth(_text.c_str());
//}
//
//int MHUIElement::getTextHeight() {
//    return _font->getHeight();
//}
//
//const std::string& MHUIElement::getText() {
//    return _text;
//}
//
//void MHUIElement::setText(const std::string& text) {
//    _text = text;
//}
//
//int MHUIElement::splitTextAt(const std::string& text, int maxWidth) {
//    return _font ? _font->splitTextAt(text, maxWidth) : 0;
//}
//
//int MHUIElement::getXOffset() {
//    return _xOffset;
//}
//
//void MHUIElement::setXOffset(int offset) {
//    _xOffset = offset;
//}
//
//int MHUIElement::getYOffset() {
//    return _yOffset;
//}
//
//void MHUIElement::setYOffset(int offset) {
//    _yOffset = offset;
//}
//
//void MHUIElement::setParent(MHUIElement *parent) {
//    _parent = parent;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//MHUIElement::MHUIElement():
//    Entity("", "MHUIElement"),
//    _manager(NULL),
//    _font(NULL),
//    _text(""),
//    _name(""),
//    _xoffset(0),
//    _yoffset(0),
//    _width(0),
//    _height(0),
//    _onTop(false),
//    _border(0)
//{
//
//}
//
//MHUIElement::~MHUIElement() {
//    cullChildren();
//}
//
//void MHUIElement::initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font) {
//    _manager = manager;
//    _font = font;
//    _name = name;
//
//    _material = mat;
//
//    VertexArray *elementArray = new VertexArray();
//    elementArray->addAttributeBuffer("position", new AttributeBuffer(NULL, GL_INT, 2, 4, _elementPositions));
//    elementArray->addAttributeBuffer("texcoord", new AttributeBuffer(NULL, GL_INT, 2, 4, _elementTexCoords));
//    _renderOp = new RenderOperation(QUADS, elementArray);
//
//}
//
//bool MHUIElement::cullChild(MHUIElement *child) {
//    if(_children.empty()) {
//        return false;
//    }
//
//    std::list<MHUIElement*>::iterator itr = _children.begin();
//    std::list<MHUIElement*>::iterator del = _children.end();
//    for(; itr != _children.end(); itr++) {
//        if ((*itr) == child) {
//            del = itr;
//            break;
//        }
//        else if((*itr)->cullChild(child)) {
//            break;
//        }
//    }
//
//    if (del != _children.end()) {
//        _children.erase(del);
//        return true;
//    }
//    else {
//        return false;
//    }
//}
//
//void MHUIElement::cullChildren() {
//    std::list<MHUIElement*>::iterator itr = _children.begin();
//    for(; itr != _children.end(); itr++) {
//        (*itr)->cullChildren();
//    }
//
//    _children.clear();
//}
//
//std::list<MHUIElement*> MHUIElement::enqueue() {
//    std::list<MHUIElement*> deferred;
//
//    if(!_onTop) {
//        RenderQueue::Get()->add(this);
//    } else {
//        deferred.push_back(this);
//    }
//
//    std::list<MHUIElement*>::iterator it;
//    for(it=_children.begin(); it!=_children.end(); it++) {
//        std::list<MHUIElement*> retElems;
//        retElems = (*it)->enqueue();
//        deferred.insert(deferred.end(), retElems.begin(), retElems.end());
//    }
//
//    return deferred;
//}
//
//void MHUIElement::render(RenderContext* context) {
//
//    
//
//
//
//    glPushMatrix();
//    glTranslatef(_position[0], _position[1], _position[2]);
//
//    if (getMaterial()) { 
//
//        getMaterial()->enableMaterial();
//        glBegin(GL_QUADS);
//            glTexCoord2f(0, 0); glVertex2i(0.0f,   0.0f   );
//            glTexCoord2f(1, 0); glVertex2i(_width, 0.0f   );
//            glTexCoord2f(1, 1); glVertex2i(_width, _height);
//            glTexCoord2f(0, 1); glVertex2i(0.0f,   _height);
//        glEnd();
//        getMaterial()->disableMaterial();
//
//        if(_border > 0) {
//            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
//            glDisable(GL_TEXTURE_2D);
//            glBegin(GL_QUADS);
//                // Bottom border
//                glVertex2i(_border,        0.0f   );
//                glVertex2i(_width-_border, 0.0f   );
//                glVertex2i(_width-_border, _border);
//                glVertex2i(_border,        _border);
//
//                // Top border
//                glVertex2i(_border,        _height        );
//                glVertex2i(_width-_border, _height        );
//                glVertex2i(_width-_border, _height-_border);
//                glVertex2i(_border,        _height-_border);
//
//                // Left border
//                glVertex2i(_border, 0.0f   );
//                glVertex2i(0.0f,    0.0f   );
//                glVertex2i(0.0f,    _height);
//                glVertex2i(_border, _height);
//
//                // Right border
//                glVertex2i(_width,         0.0f   );
//                glVertex2i(_width-_border, 0.0f   );
//                glVertex2i(_width-_border, _height);
//                glVertex2i(_width,         _height);
//            glEnd();
//        }
//    }
//    
//    if(_text.length()>0) {
//        _font->print(_position[0]+_xoffset, _position[1]+_yoffset,
//                     _manager->getWidth(), _manager->getHeight(), _text.data());
//    }  
//
//    glPopMatrix();
//}
//
//void MHUIElement::addChild(MHUIElement* child) {
//    _children.push_back(child);
//}
//
//const std::list<MHUIElement*> &MHUIElement::getChildren() { return _children; }
//
//int MHUIElement::getTextWidth() { return _font->getWidth(_text.c_str()); }
//int MHUIElement::getTextHeight() { return _font->getHeight(); }
//
//int MHUIElement::splitTextAt(const std::string& text, int maxWidth) { return _font->splitTextAt(text, maxWidth); }
//
//void MHUIElement::setX(int x) { _position[0] = x; }
//void MHUIElement::setY(int y) { _position[1] = y; }
//
//const std::string& MHUIElement::getText() { return _text; }
//void MHUIElement::setText(const std::string& text) { _text = text; }
//
//int MHUIElement::getXOffset() { return _xoffset; }
//void MHUIElement::setXOffset(int offset) { _xoffset = offset; }
//
//int MHUIElement::getYOffset() { return _yoffset; }
//void MHUIElement::setYOffset(int offset) { _yoffset = offset; }
//
//int MHUIElement::getWidth() { return _width; }
//void MHUIElement::setWidth(int width) { _width = width; }
//
//int MHUIElement::getHeight() { return _height; }
//void MHUIElement::setHeight(int height) { _height = height; }
//
//int MHUIElement::getBorder() { return _border; }
//void MHUIElement::setBorder(int border) { _border = border; }
//
//bool MHUIElement::getAlwaysOnTop() { return _onTop; }
//void MHUIElement::setAlwaysOnTop(bool val) { _onTop = val; }
