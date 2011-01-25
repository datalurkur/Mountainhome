/*
 *  PixelData.h
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _PIXELDATA_H_
#define _PIXELDATA_H_
#include "GL_Helper.h"
#include <string>

/*! \todo Make this more powerful by including width/height.
 *  \todo Remove references to GL_Helper */
class PixelData {
public:
    PixelData(): layout(GL_RGBA), type(GL_UNSIGNED_BYTE), pixels(0) {}
    PixelData(GLenum layout_, GLenum type_, void *pixels_): layout(layout_), type(type_), pixels(pixels_) {}

    void saveToDisk(const std::string &name, int width, int height);

    GLenum layout;
    GLenum type;
    void *pixels;
};

#endif
