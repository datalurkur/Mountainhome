/*
 *  Texture.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "GL_Helper.h"
#include "PixelData.h"
#include <string>

class TextureManager;
/*! \brief A texture stored in video memory that may be activated or deactivated.
    \todo Does it make sense for the Texture to be able to activate itself?
    \todo Remove SDL_Helper.h */
class Texture {
protected:
    friend class TextureManager;
    template <typename Resource> friend class ResourceManager;

    Texture(GLenum target, GLuint *ids, int frames, int w, int h, int d);
    virtual ~Texture();

    void setInternals(GLenum target, GLuint *ids, int frames, int w, int h, int d);
    void initEnvironment();

public:
    static void CalcMipMapSize(int level, int &width, int &height, int &depth);
    static GLenum DefaultMinFilter;
    static GLenum DefaultMagFilter;
    static GLenum DefaultTextureEnv;
    static GLenum DefaultSCoordHandling;
    static GLenum DefaultTCoordHandling;
    static GLenum DefaultRCoordHandling;

public:
    int width();
    int height();
    int depth();

    int dimensions();
    
    GLuint id(int frame = 0);
    GLuint target();

    void bind(int level = 0, int frame = 0);
    void release(int level = 0);
    void bindAndEnable(int level = 0, int frame = 0);
    void releaseAndDisable(int level = 0);

    void setEnvironment(GLenum env);
    void setFiltering(GLenum minFilter, GLenum magFilter);
    void setTexCoordHandling(GLenum sCoord, GLenum tCoord, GLenum rCoord = GL_REPEAT);
    void setAnisoLevel(int level);

    void uploadPixelData(const PixelData &data, int level = -1, int frame = 0);
    void downloadPixelData(PixelData &data, int level = 0, int frame = 0);

protected:
    int _width;
    int _height;
    int _depth;
    
    GLenum _target;
    GLuint *_textureId;
    int _numFrames;
};

#endif
