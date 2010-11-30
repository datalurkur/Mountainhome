/*
 *  MHUIElement.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHUIElement.h"

#include <Render/MaterialManager.h>
#include <Render/FontManager.h>
#include <Render/Font.h>

MHUIElement::MHUIElement(): Entity(""), _manager(NULL), _font(NULL), _text(""), _name(""),
_xoffset(0), _yoffset(0), _width(0), _height(0), _onTop(false), _border(0) {}

MHUIElement::~MHUIElement() {
    cullChildren();
}

void MHUIElement::initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font) {
    _manager = manager;
    _font = font;
    _name = name;

    Renderable::setMaterial(mat);
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
        RenderQueue::Get()->add(this);
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

        context->setActiveMaterial(getMaterial());
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(0.0f,   0.0f   );
            glTexCoord2f(1, 0); glVertex2i(_width, 0.0f   );
            glTexCoord2f(1, 1); glVertex2i(_width, _height);
            glTexCoord2f(0, 1); glVertex2i(0.0f,   _height);
        glEnd();
        context->unsetActiveMaterial(getMaterial());

        if(_border > 0) {
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
                // Bottom border
                glVertex2i(_border,        0.0f   );
                glVertex2i(_width-_border, 0.0f   );
                glVertex2i(_width-_border, _border);
                glVertex2i(_border,        _border);

                // Top border
                glVertex2i(_border,        _height        );
                glVertex2i(_width-_border, _height        );
                glVertex2i(_width-_border, _height-_border);
                glVertex2i(_border,        _height-_border);

                // Left border
                glVertex2i(_border, 0.0f   );
                glVertex2i(0.0f,    0.0f   );
                glVertex2i(0.0f,    _height);
                glVertex2i(_border, _height);

                // Right border
                glVertex2i(_width,         0.0f   );
                glVertex2i(_width-_border, 0.0f   );
                glVertex2i(_width-_border, _height);
                glVertex2i(_width,         _height);
            glEnd();
        }
    }
    
    if(_text.length()>0) {
        _font->print(_position[0]+_xoffset, _position[1]+_yoffset,
                     _manager->getWidth(), _manager->getHeight(), _text.data());
    }  

    glPopMatrix();
}

void MHUIElement::addChild(MHUIElement* child) {
    _children.push_back(child);
}

const std::list<MHUIElement*> &MHUIElement::getChildren() { return _children; }

int MHUIElement::getTextWidth() { return _font->getWidth(_text.c_str()); }
int MHUIElement::getTextHeight() { return _font->getHeight(); }

int MHUIElement::splitTextAt(const std::string& text, int maxWidth) { return _font->splitTextAt(text, maxWidth); }

void MHUIElement::setX(int x) { _position[0] = x; }
void MHUIElement::setY(int y) { _position[1] = y; }

const std::string& MHUIElement::getText() { return _text; }
void MHUIElement::setText(const std::string& text) { _text = text; }

int MHUIElement::getXOffset() { return _xoffset; }
void MHUIElement::setXOffset(int offset) { _xoffset = offset; }

int MHUIElement::getYOffset() { return _yoffset; }
void MHUIElement::setYOffset(int offset) { _yoffset = offset; }

int MHUIElement::getWidth() { return _width; }
void MHUIElement::setWidth(int width) { _width = width; }

int MHUIElement::getHeight() { return _height; }
void MHUIElement::setHeight(int height) { _height = height; }

int MHUIElement::getBorder() { return _border; }
void MHUIElement::setBorder(int border) { _border = border; }

bool MHUIElement::getAlwaysOnTop() { return _onTop; }
void MHUIElement::setAlwaysOnTop(bool val) { _onTop = val; }
