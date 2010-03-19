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

class PixelData {
public:
    PixelData(): internal(GL_RGBA), format(GL_RGBA), type(GL_UNSIGNED_BYTE), pixels(0) {}
    PixelData(GLenum i, GLenum f, GLenum t, void *p): internal(i), format(f), type(t), pixels(p) {}
    PixelData(GLenum i, GLenum t, void *p): internal(i), format(i), type(t), pixels(p) {}

#ifdef USEPNGLIB
    void saveToDisk(const std::string &name);
#endif

    GLenum internal;
    GLenum format;
    GLenum type;
    void *pixels;
};

#endif
