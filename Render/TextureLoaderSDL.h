/*
 *  TextureLoaderSDL.h
 *  Base
 *
 *  Created by Brent Wilson on 11/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 *  TextureLoaderSDL is an image loader wrapper class for Texture
 *
 */

#ifndef _TEXTURELOADERSDL_H_
#define _TEXTURELOADERSDL_H_
#include <string>

class Texture;
class TextureLoaderSDL {
public:
    static bool CanLoad(const std::string &name);
    static Texture* Load(const std::string &name);
    static Texture* LoadCubeMap(const std::string &name, const std::string files[6]);
    static Texture* LoadAnimated(const std::string &name, int n, const std::string *files);

};

#endif
