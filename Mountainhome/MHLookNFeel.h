/*
 * MHLookNFeel.h
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2011 Mountainhome Project.  All rights reserved.
 *
 */

#ifndef _MHLOOKNFEEL_H_
#define _MHLOOKNFEEL_H_

#include "Renderable.h"

class MHLookNFeel {
public:
    Renderable *createRectRenderable(int width, int height, int xoffset, int yoffset, const std::string &materialName);
    Renderable *createTextRenderable(const std::string &fontName, Color4 color, const std::string &text);

    int getTextWidth(const std::string &fontName, const std::string &text);
    int getTextHeight(const std::string &fontName);

    void splitTextAt(const std::string &fontName, const std::string &text, int width, std::vector<std::string> &snippets);

public:
    MHLookNFeel();
    virtual ~MHLookNFeel() {}
};

#endif
