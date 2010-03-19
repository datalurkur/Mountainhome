/*
 *  TextureSDL.h
 *  Base
 *
 *  Created by Brent Wilson on 11/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 *  TextureSDL is an image Factory wrapper class for Texture
 *
 */

#ifndef _TEXTUREFACTORYSDL_H_
#define _TEXTUREFACTORYSDL_H_
#include "Texture.h"

/*! \todo Support cube maps, animated textures, and 3D textures. */
class TextureSDL : public Texture {
public:
    static void Setup();
    static void Teardown();
    static bool CanLoad(const std::string &args);
    static Texture* Load(const std::string &args);

protected:
    TextureSDL(const std::string &args);
    virtual ~TextureSDL();

};

#endif
