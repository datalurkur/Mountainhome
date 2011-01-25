/*
 *  Texture.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Texture.h"
#include "TextureManager.h"
#include <Base/Assertion.h>
#include <Base/Math3D.h>
#include "PixelData.h"

GLenum Texture::DefaultMinFilter = GL_NEAREST_MIPMAP_NEAREST;
GLenum Texture::DefaultMagFilter = GL_NEAREST;
GLenum Texture::DefaultTextureEnv = GL_MODULATE;
GLenum Texture::DefaultSCoordHandling = GL_REPEAT;
GLenum Texture::DefaultTCoordHandling = GL_REPEAT;
GLenum Texture::DefaultRCoordHandling = GL_REPEAT;

void Texture::CalcMipMapSize(int level, int &width, int &height, int &depth) {
    if (level > 0) {
        width  = Math::Max(1, width >> level);
        height = Math::Max(1, height >> level);
        depth  = Math::Max(1, depth >> level);
    }
}

Texture::Texture(GLenum target, GLuint *ids, int frames, int w, int h, int d) {
    setInternals(target, ids, frames, w, h, d);
}

Texture::~Texture() {
    if (_textureId) {
        glDeleteTextures(_numFrames, _textureId);
        delete[] _textureId;
    }
}

void Texture::initEnvironment() {
    ASSERT(getWidth()  > 0);
    ASSERT(getHeight() > 0);
    ASSERT(getDepth()  > 0);
    ASSERT(_numFrames  > 0);

    _internalFormat = 0;
    glTexParameterf(getTarget(), GL_TEXTURE_MIN_FILTER, DefaultMinFilter);
    glTexParameterf(getTarget(), GL_TEXTURE_MAG_FILTER, DefaultMagFilter);
    glTexParameterf(getTarget(), GL_TEXTURE_WRAP_S, DefaultSCoordHandling);
    glTexParameterf(getTarget(), GL_TEXTURE_WRAP_T, DefaultTCoordHandling);
    glTexParameterf(getTarget(), GL_TEXTURE_WRAP_R, DefaultRCoordHandling);
}

GLenum Texture::getFormat() {
    return _internalFormat;
}

int Texture::dimensions() {
    if (getDepth()  > 1) { return 3; }
    if (getHeight() > 1) { return 2; }
    return 1;
}

void Texture::setInternals(GLenum target, GLuint *id, int frames, int w, int h, int d) {
    _target = target;
    _textureId = id;
    _numFrames = frames;
    _width = w;
    _height = h;
    _depth = d;

    for (int i = 0; i < frames; i++) {
        enable(0, i);
        initEnvironment();
        disable(i);
    }
    
}

void Texture::enable(int level, int frame) {
    glActiveTexture(GL_TEXTURE0 + level);
    glBindTexture(_target, _textureId[frame]);
    glEnable(_target);
}

void Texture::disable(int level) {
    glActiveTexture(GL_TEXTURE0 + level);
    glBindTexture(_target, 0);
    glDisable(_target);
}

void Texture::setFiltering(GLenum minFilter, GLenum magFilter) {
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(_target, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameterf(_target, GL_TEXTURE_MAG_FILTER, magFilter);
    }
}

void Texture::setTexCoordHandling(GLenum sCoord, GLenum tCoord, GLenum rCoord) {
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(_target, GL_TEXTURE_WRAP_S, sCoord);
        glTexParameterf(_target, GL_TEXTURE_WRAP_T, tCoord);
        glTexParameterf(_target, GL_TEXTURE_WRAP_R, rCoord);
    }
}

void Texture::setAnisoLevel(int level) {
    float largest;

    // Makesure the given aniso level isn't too large. If it is, cap it and make a note.
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
    if (level > largest) {
        Warn("Aniso level of " << level << " is to large. Max is " << largest << ".");
        level = largest;
    }

    // Set the aniso level.
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
    }
}

int Texture::getWidth()  { return _width;  }
int Texture::getHeight() { return _height; }
int Texture::getDepth()  { return _depth;  }

GLenum Texture::getTarget() { return _target; }
GLuint Texture::getID(int frame) {
    ASSERT(frame < _numFrames);
    return _textureId[frame];
}

void Texture::uploadPixelData(const PixelData &data, GLenum internal, int level, int frame) {
    enable(0, frame);
    CalcMipMapSize(level, _width, _height, _depth);

    if (data.layout == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        data.layout == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
        data.layout == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        _internalFormat = data.layout;

        if (level < 0) {
            Error("No mipmaps on precompressed textures");
            return;
        } else {
            switch (dimensions()) {
            case 1: glCompressedTexImage1D(getTarget(), level, data.layout, getWidth(), 0, data.type * ceil(getWidth() / 4.0), data.pixels); break;
            case 2: glCompressedTexImage2D(getTarget(), level, data.layout, getWidth(), getHeight(), 0, data.type * ceil(getWidth() / 4.0) * ceil(getHeight() / 4.0), data.pixels); break;
            case 3: glCompressedTexImage3D(getTarget(), level, data.layout, getWidth(), getHeight(), getDepth(), 0, data.type * ceil(getWidth() / 4.0) * ceil(getHeight() / 4.0) * ceil(getDepth() / 4.0), data.pixels); break;
            }
        }
    } else {
        _internalFormat = internal ?: data.layout;

        if (level < 0) {
            switch (dimensions()) {
            case 1: gluBuild1DMipmaps(getTarget(), _internalFormat, getWidth(), data.layout, data.type, data.pixels); break;
            case 2: gluBuild2DMipmaps(getTarget(), _internalFormat, getWidth(), getHeight(), data.layout, data.type, data.pixels); break;
            case 3: gluBuild3DMipmaps(getTarget(), _internalFormat, getWidth(), getHeight(), getDepth(), data.layout, data.type, data.pixels); break;
            }
        } else {
            switch (dimensions()) {
            case 1: glTexImage1D(getTarget(), level, _internalFormat, getWidth(), 0, data.layout, data.type, data.pixels); break;
            case 2: glTexImage2D(getTarget(), level, _internalFormat, getWidth(), getHeight(), 0, data.layout, data.type, data.pixels); break;
            case 3: glTexImage3D(getTarget(), level, _internalFormat, getWidth(), getHeight(), getDepth(), 0, data.layout, data.type, data.pixels); break;
            }
        }
    }

    disable();
}


/*
    enum TextureEnums {
        //Textures
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_3D,

        //Environment
        MODULATE,
        DECAL,
        BLEND,
        REPLACE,

        //Filters
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR,

        //Formats
        ALPHA,
        LUMINANCE,
        INTENSITY,
        RGB,
        RGBA,

        //Wrap Modes
        CLAMP,
        CLAMP_TO_EDGE,
        REPEAT
    };

GLenum getGLEnum(GLenum old) {
    switch(old) {
        case Texture::TEXTURE_1D:                return GL_TEXTURE_1D;
        case Texture::TEXTURE_2D:                return GL_TEXTURE_2D;
        case Texture::TEXTURE_3D:                return GL_TEXTURE_3D;

        //Environment
        case Texture::MODULATE:                    return GL_MODULATE;
        case Texture::DECAL:                    return GL_DECAL;
        case Texture::BLEND:                    return GL_BLEND;
        case Texture::REPLACE:                    return GL_REPLACE;

        //Filters
        case Texture::NEAREST:                    return GL_NEAREST;
        case Texture::LINEAR:                    return GL_LINEAR;
        case Texture::NEAREST_MIPMAP_NEAREST:    return GL_NEAREST_MIPMAP_NEAREST;
        case Texture::LINEAR_MIPMAP_NEAREST:    return GL_LINEAR_MIPMAP_NEAREST;
        case Texture::NEAREST_MIPMAP_LINEAR:    return GL_NEAREST_MIPMAP_LINEAR;
        case Texture::LINEAR_MIPMAP_LINEAR:        return GL_LINEAR_MIPMAP_LINEAR;

        //Formats
        case Texture::ALPHA:                    return GL_ALPHA;
        case Texture::LUMINANCE:                return GL_LUMINANCE;
        case Texture::INTENSITY:                return GL_INTENSITY;
        case Texture::RGB:                        return GL_RGB;
        case Texture::RGBA:                        return GL_RGBA;

        //Wrap Modes
        case Texture::CLAMP:                    return GL_CLAMP;
        case Texture::CLAMP_TO_EDGE:            return GL_CLAMP_TO_EDGE;
        case Texture::REPEAT:                    return GL_REPEAT;
    }

    Error("INVALID ENUM!");
    return 0;
}*/

