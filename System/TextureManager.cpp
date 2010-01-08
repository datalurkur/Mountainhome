/*
 *  TextureManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Base/FileSystem.h>
#include <Base/Assertion.h>
#include <Base/Math3D.h>
#include <Base/File.h>

#include "SDLTexture.h"
#include "TextureManager.h"
#include "Texture.h"

TextureManager::TextureManager() {
    _defaultMinFilter = GL_NEAREST_MIPMAP_NEAREST;
    _defaultMagFilter = GL_NEAREST;
    _defaultTextureEnv = GL_MODULATE;
    _defaultSCoordHandling = GL_REPEAT;
    _defaultTCoordHandling = GL_REPEAT;
    _defaultRCoordHandling = GL_REPEAT;

    registerLoader<SDLTexture>();
}

TextureManager::~TextureManager() {}

void TextureManager::calcMipMapSize(int level, int &width, int &height, int &depth) {
    if (level > 0) {
        width  = Math::Max(1, width >> level);
        height = Math::Max(1, height >> level);
        depth  = Math::Max(1, depth >> level);
    }
}

void TextureManager::initEnvironment(GLenum target) {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, _defaultTextureEnv);
    glTexParameterf(target, GL_TEXTURE_MIN_FILTER, _defaultMinFilter);
    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, _defaultMagFilter);
    glTexParameterf(target, GL_TEXTURE_WRAP_S, _defaultSCoordHandling);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, _defaultTCoordHandling);
    glTexParameterf(target, GL_TEXTURE_WRAP_R, _defaultRCoordHandling);
}

GLuint* TextureManager::initTexture(GLenum target, int frames) {
    if (frames < 1) {
        Error("Cannot init a texture with fewer than 1 frame");
        return NULL;
    }

       GLuint *ids = new GLuint[frames];
    glGenTextures(frames, ids);
    for (int i = 0; i < frames; i++) {
        glBindTexture(target, ids[i]);
        initEnvironment(target);
    }

    return ids;
}

Texture* TextureManager::init1D(const string &name, int w, int frames) {
    Texture *tex = new Texture();
    if (!init1D(tex, name, w, frames)) {
        delete tex;
        tex = NULL;
    }
    return tex;
}

Texture* TextureManager::init2D(const string &name, int w, int h, int frames) {
    Texture *tex = new Texture();
    if (!init2D(tex, name, w, h, frames)) {
        delete tex;
        tex = NULL;
    }
    return tex;
}

Texture* TextureManager::init3D(const string &name, int w, int h, int d, int frames) {
    Texture *tex = new Texture();
    if (!init3D(tex, name, w, h, d, frames)) {
        delete tex;
        tex = NULL;
    }
    return tex;
}

Texture* TextureManager::initCube(const string &name, int w, int h, int frames) {
    Texture *tex = new Texture();
    if (!initCube(tex, name, w, h, frames)) {
        delete tex;
        tex = NULL;
    }

    return tex;
}

bool TextureManager::init1D(Texture *tex, const string &name, int w, int frames) {
    GLuint *ids = initTexture(GL_TEXTURE_1D, frames);
    if (ids) {
        tex->setInternals(GL_TEXTURE_1D, ids, frames, w, 1, 1);
        registerResource(name, tex);
        return true;
    }

    return false;
}

bool TextureManager::init2D(Texture *tex, const string &name, int w, int h, int frames) {
    GLuint *ids = initTexture(GL_TEXTURE_2D, frames);
    if (ids) {
        tex->setInternals(GL_TEXTURE_2D, ids, frames, w, h, 1);
        registerResource(name, tex);
        return true;
    }

    return false;
}

bool TextureManager::init3D(Texture *tex, const string &name, int w, int h, int d, int frames) {
    GLuint *ids = initTexture(GL_TEXTURE_3D, frames);
    if (ids) {
        tex->setInternals(GL_TEXTURE_3D, ids, frames, w, h, d);
        registerResource(name, tex);
        return true;
    }

    return false;
}

bool TextureManager::initCube(Texture *tex, const string &name, int w, int h, int frames) {
    GLuint *ids = initTexture(GL_TEXTURE_CUBE_MAP, frames);
    if (ids) {
        tex->setInternals(GL_TEXTURE_CUBE_MAP, ids, frames, w, h, 1);
        registerResource(name, tex);
        return true;
    }

    return false;
}

void TextureManager::blank1D(Texture *tex, GLenum internal, int level, int frame) {
    tex->bind(0, frame);
    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
        format = GL_DEPTH_COMPONENT;
    }

    glTexImage1D(tex->target(), 0, internal, tex->width(), 0, format, GL_FLOAT, NULL);
    tex->release();
}

void TextureManager::blank2D(Texture *tex, GLenum internal, int level, int frame) {
    tex->bind(0, frame);
    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
        format = GL_DEPTH_COMPONENT;
    }

    glTexImage2D(tex->target(), 0, internal, tex->width(), tex->height(), 0, format,
                 GL_FLOAT, NULL);
    tex->release();
}

void TextureManager::blank3D(Texture *tex, GLenum internal, int level, int frame) {
    tex->bind(0, frame);
    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
        format = GL_DEPTH_COMPONENT;
    }

    glTexImage3D(tex->target(), 0, internal, tex->width(),
                          tex->height(), tex->depth(), 0, format, GL_FLOAT, NULL);
    tex->release();
}

void TextureManager::blankCube(Texture *tex, GLenum internal, int level, int frame) {
    tex->bind(0, frame);
    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
        format = GL_DEPTH_COMPONENT;
    }

    for (int i = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; i < GL_TEXTURE_CUBE_MAP_NEGATIVE_X + 6; i++) {
        glTexImage2D(i, 0, internal, tex->width(),
                     tex->height(), 0, format, GL_INT, NULL);
    }

    tex->release();
}

void TextureManager::tex1D(Texture *tex, PixelData &data, int level, int frame) {
    tex->bind(0, frame);
    int width  = tex->width();
    int height = tex->height();
    int depth  = tex->depth();
    calcMipMapSize(level, width, height, depth);

    if (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        if (level < 0) {
            Error("No mipmaps on precompressed textures");
            return;
        } else {
            glCompressedTexImage1D(tex->target(), level, data.format, width, 0,
                                   data.type * ceil(width / 4.0), data.pixels);
        }
    } else {
        if (level < 0) {
            gluBuild1DMipmaps(tex->target(), data.internal, width, data.format, data.type,
                              data.pixels);
        } else {
            glTexImage1D(tex->target(), level, data.internal, width, 0, data.format,
                         data.type, data.pixels);
        }
    }

    tex->release();
}

void TextureManager::tex2D(Texture *tex, PixelData &data, int level, int frame) {
    tex->bind(0, frame);
    int width  = tex->width();
    int height = tex->height();
    int depth  = tex->depth();
    calcMipMapSize(level, width, height, depth);

    if (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        if (level < 0) {
            Error("No mipmaps on precompressed textures");
            return;
        } else {
            glCompressedTexImage2D(tex->target(), level, data.format, width, height, 0,
                                   data.type * ceil(width / 4.0) * ceil(height / 4.0),
                                   data.pixels);
        }
    } else {
        if (level < 0) {
            gluBuild2DMipmaps(tex->target(), data.internal, width, height, data.format,
                              data.type, data.pixels);
        } else {
            glTexImage2D(tex->target(), level, data.internal, width, height, 0,
                         data.format, data.type, data.pixels);
        }
    }

    tex->release();
}

void TextureManager::tex3D(Texture *tex, PixelData &data, int level, int frame) {
    tex->bind(0, frame);
    int width  = tex->width();
    int height = tex->height();
    int depth  = tex->depth();
    calcMipMapSize(level, width, height, depth);

    if (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        if (level < 0) {
            Error("No mipmaps on precompressed textures");
            return;
        } else {
            glCompressedTexImage3D(tex->target(), level, data.format, width, height,
                                   depth, 0, data.type * ceil(width / 4.0) *
                                   ceil(height / 4.0) * ceil(depth / 4.0), data.pixels);
        }
    } else {
        if (level < 0) {
            gluBuild3DMipmaps(tex->target(), data.internal, width, height, depth,
                              data.format, data.type, data.pixels);
        } else {
            glTexImage3D(tex->target(), level, data.internal, width, height, depth, 0,
                         data.format, data.type, data.pixels);
        }
    }

    tex->release();
}

void TextureManager::texCube(Texture *tex, PixelData &data, GLenum face, int level,
                             int frame) {
    tex->bind(0, frame);
    int width  = tex->width();
    int height = tex->height();
    int depth  = tex->depth();
    calcMipMapSize(level, width, height, depth);

    if (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
        data.format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        if (level < 0) {
            Error("No mipmaps on precompressed textures");
            return;
        } else {
            glCompressedTexImage2D(face, level, data.format, width, height, 0, data.type *
                                   ceil(width / 4) * ceil(height / 4), data.pixels);
        }
    } else {
        if (level < 0) {
            gluBuild2DMipmaps(face, data.internal, width, height, data.format, data.type,
                              data.pixels);
        } else {
            glTexImage2D(face, level, data.internal, width, height, 0, data.format,
                         data.type, data.pixels);
        }
    }

    tex->release();
}
