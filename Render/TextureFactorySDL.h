/*
 *  TextureFactorySDL.h
 *  Base
 *
 *  Created by Brent Wilson on 11/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 *  TextureFactorySDL is an image Factory wrapper class for Texture
 *
 */

#ifndef _TEXTUREFACTORYSDL_H_
#define _TEXTUREFACTORYSDL_H_
#include <Base/ResourceManager.h>

class Texture;
class TextureFactorySDL : public ResourceFactory<Texture> {
public:
    bool canLoad(const std::string &name);
    Texture* load(const std::string &name);
    Texture* loadCubeMap(const std::string &name, const std::string files[6]);
    Texture* loadAnimated(const std::string &name, int n, const std::string *files);

};

#endif
