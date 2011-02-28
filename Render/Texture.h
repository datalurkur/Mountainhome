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
    \todo Remove GL_Helper.h */
class Texture {
public:
    static void CalcMipMapSize(int level, int &width, int &height, int &depth);

    static GLenum DefaultMinFilter;
    static GLenum DefaultMagFilter;
    static GLenum DefaultTextureEnv;
    static GLenum DefaultSCoordHandling;
    static GLenum DefaultTCoordHandling;
    static GLenum DefaultRCoordHandling;

public:
    Texture(int frames = 1);
    Texture(const std::string &name, int frames = 1);
    virtual ~Texture();

    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getDepth();

    int dimensions();
    
    GLuint getID(int frame = 0);
    GLuint getTarget();
    GLenum getFormat();

    void enable(int level = 0, int frame = 0);
    void disable(int level = 0);

    void setFiltering(GLenum minFilter, GLenum magFilter);
    void setTexCoordHandling(GLenum sCoord, GLenum tCoord, GLenum rCoord = GL_REPEAT);
    void setAnisoLevel(int level);

    void uploadPixelData(
        const PixelData &data,
        unsigned int w,
        unsigned int h,
        unsigned int d = 1,
        GLenum format = 0,
        bool genMipmaps = true,
        int frame = 0);

protected:
    void initEnvironment();

protected:
    unsigned int _width;
    unsigned int _height;
    unsigned int _depth;
    
    GLenum _target;
    GLenum _internalFormat;
    GLuint *_textureId;
    int _numFrames;

    std::string _name;
};

#endif
