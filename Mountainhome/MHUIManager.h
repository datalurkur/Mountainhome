/*
 *  MHUIManager.h
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHUIMANAGER_H_
#define _MHUIMANAGER_H_
#include "MHUIElement.h"
#include "MHCore.h"
#include <string>

class MHUIElement;
class Font;

class MHUIManager {
public:
    MHUIManager();
    virtual ~MHUIManager();

    void render(RenderContext *context);

	Font *getFont() { return _font; }
    void setWidth(int width) { _width = width; }
    int getWidth() { return _width; }
    void setHeight(int height) { _height = height; }
    int getHeight() { return _height; }

    MHUIElement *getRoot();
    void setRoot(MHUIElement *node);

private:
    MHUIElement *_rootNode;

    Font *_font;
    int _width, _height;

};

#endif
