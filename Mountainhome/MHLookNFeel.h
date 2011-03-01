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

#include "MHUIElement.h"

class MHLookNFeel {
public:
    void clearRenderables(MHUIElement *element);
    void addRectRenderable(MHUIElement *element, int width, int height, const std::string &materialName);
    void addTextRenderable(MHUIElement *element, const std::string &fontName, const std::string &text);

    int getTextWidth(const std::string &fontName, const std::string &text);
    int getTextHeight(const std::string &fontName);

public:
    MHLookNFeel();
    virtual ~MHLookNFeel() {}
};

#endif
