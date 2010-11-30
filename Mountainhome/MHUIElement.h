/*
 *  MHUIElement.h
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include "Renderable.h"
#include <Render/RenderContext.h>
#include "MHUIManager.h"

class MHUIManager;
class Font;

class MHUIElement: public Entity, public Renderable {
public:
    // DO NOT CREATE MHUIElement IN C!!!!! ONLY RUBY!!!
    MHUIElement();
    virtual ~MHUIElement();

    void initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font);

    bool cullChild(MHUIElement *child);
    void cullChildren();

    std::list<MHUIElement*> enqueue();
    void render(RenderContext *context);

    const std::string& getText();
    void setText(const std::string& text);

    int getTextWidth();
    int getTextHeight();

    int splitTextAt(const std::string& text, int maxWidth);

    void setX(int x);
    void setY(int y);

    int getXOffset();
    void setXOffset(int offset);

    int getYOffset();
    void setYOffset(int offset);

    int getWidth();
    void setWidth(int width);

    int getHeight();
    void setHeight(int height);

    bool getAlwaysOnTop();
    void setAlwaysOnTop(bool val);

    int getBorder();
    void setBorder(int border);

    void addChild(MHUIElement* child);

    const std::list<MHUIElement*> &getChildren();

private:
	MHUIManager *_manager;
    Font *_font;

    std::string _text, _name;
    int _xoffset, _yoffset;
    int _width, _height;
    int _border;
    bool _onTop;

    std::list<MHUIElement*> _children;
};

#endif

