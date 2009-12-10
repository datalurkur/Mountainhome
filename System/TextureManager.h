/*
 *  TextureManager.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_
#include "SingleFileManager.h"
#include "Singleton.h"
#include "GL_Helper.h"

class Texture;

/*! \brief Provides the functionality for Texture caching and Loading.
    \todo Remove GL_Helper.h
    \author Brent Wilson
    \date 4/22/07 */
class TextureManager : public SingleFileManager<Texture>, public Singleton<TextureManager> {
public:
    struct PixelData {
        PixelData(): internal(GL_RGBA), format(GL_RGBA), type(GL_UNSIGNED_BYTE),
                     pixels(NULL) {}
        PixelData(GLenum i, GLenum f, GLenum t, void *p): internal(i), format(f), type(t),
                                                          pixels(p) {}
        PixelData(GLenum i, GLenum t, void *p): internal(i), format(i), type(t),
                                                pixels(p) {}

        GLenum internal;
        GLenum format;
        GLenum type;
        void *pixels;
    };

public:
    Texture* loadCubeMap(const std::string &name, const std::string files[6]);
    Texture* loadAnimated(const std::string &name, int n, const std::string *files);

    Texture* init1D(const string &name, int w, int numFrames = 1);
    Texture* init2D(const string &name, int w, int h, int numFrames = 1);
    Texture* init3D(const string &name, int w, int h, int d, int numFrames = 1);
    Texture* initCube(const string &name, int w, int h, int numFrames = 1);

    void blank1D(Texture *tex, GLenum internal, int level = 0, int frame = 0);
    void blank2D(Texture *tex, GLenum internal, int level = 0, int frame = 0);
    void blank3D(Texture *tex, GLenum internal, int level = 0, int frame = 0);
    void blankCube(Texture *tex, GLenum internal, int level = 0, int frame = 0);

    void tex1D(Texture *tex, PixelData &data, int level = -1, int frame = 0);
    void tex2D(Texture *tex, PixelData &data, int level = -1, int frame = 0);
    void tex3D(Texture *tex, PixelData &data, int level = -1, int frame = 0);
    void texCube(Texture *tex, PixelData &data, GLenum face, int level = -1, int frame = 0);

private:
    void initEnvironment(GLenum target);
    GLuint* initTexture(GLenum target, int frames);

    bool init1D(Texture *tex, const string &name, int w, int frames = 1);
    bool init2D(Texture *tex, const string &name, int w, int h, int frames = 1);
    bool init3D(Texture *tex, const string &name, int w, int h, int d, int frames = 1);
    bool initCube(Texture *tex, const string &name, int w, int h, int frames = 1);

    void killTexture(Texture* &texture);
    void calcMipMapSize(int level, int &width, int &height, int &depth);

    GLenum _defaultMinFilter;
    GLenum _defaultMagFilter;
    GLenum _defaultTextureEnv;
    GLenum _defaultSCoordHandling;
    GLenum _defaultTCoordHandling;
    GLenum _defaultRCoordHandling;

protected:
    TextureManager();
    ~TextureManager();

    template <class T> friend class Singleton;

};

#endif
