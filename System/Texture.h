/*
 *  Texture.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "GL_Helper.h"
#include <string>

class TextureManager;
/*! \brief A texture stored in video memory that may be activated or deactivated.
    \todo Does it make sense for the Texture to be able to activate itself?
    \todo Remove SDL_Helper.h
    \author Brent Wilson
    \date 4/22/07 */
class Texture {
public:
    static TextureManager* GetManager();

public:
    static void SaveTexture(Texture *texture, const std::string &name);
    static void SaveTexture(unsigned char *pixels, int width, int height,
                            const std::string &name);
public:
    friend class TextureManager;
    template <class T> friend class ResourceManager;
    
    Texture();
    ~Texture();
    Texture(GLenum target, GLuint id, int w, int h, int d);
    Texture(GLenum target, GLuint *ids, int frames, int w, int h, int d);
    void setInternals(GLenum target, GLuint id, int w, int h, int d);
    void setInternals(GLenum target, GLuint *ids, int frames, int w, int h, int d);

public:
    static Texture* Load(const std::string &name);
    static Texture* RandomTexture(int width, int height);
    static Texture* Load(int width, int height, GLenum format, GLenum type, void* pixels);

    int width();
    int height();
    int depth();
    
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
    
protected:
    int _width;
    int _height;
    int _depth;
    
    GLenum _target;
    GLuint *_textureId;
    int _numFrames;
};

#endif
