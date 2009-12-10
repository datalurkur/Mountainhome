/*
 *  SDLTexture.h
 *  System
 *
 *  Created by loch on 11/15/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SDLTEXTURE_H_
#define _SDLTEXTURE_H_
#include <string>

class Texture;
class SDLTexture {
public:
    static bool CanLoad(const std::string &name);
    static Texture* Load(const std::string &name);
    static Texture* LoadCubeMap(const std::string &name, const std::string files[6]);
    static Texture* LoadAnimated(const std::string &name, int n, const std::string *files);

};

#endif
