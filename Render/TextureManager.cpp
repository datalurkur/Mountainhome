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

#include "TextureSDL.h"
#include "TextureManager.h"
#include "Texture.h"

TextureManager::TextureManager(ResourceGroupManager *manager) {
    registerFactory(new TextureSDL::Factory(manager, this));
}

TextureManager::~TextureManager() {}

Texture *TextureManager::createTexture(const std::string &name, int w, int h, int d, int frames) {
    GLuint *ids = new GLuint[frames];
    glGenTextures(frames, ids);
    GLenum target = GL_TEXTURE_1D;
    if (h > 1) { target = GL_TEXTURE_2D; }
    if (d > 1) { target = GL_TEXTURE_3D; }
    Texture *tex = new Texture(target, ids, frames, w, h, d);
    registerResource(name, tex);
    return tex;
}

Texture *TextureManager::createBlankTexture(const std::string &name, GLenum internal, int w, int h, int d, int frames) {
    Texture *tex = createTexture(name, w, h, d, frames);

    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
        format = GL_DEPTH_COMPONENT;
    }

    PixelData data(format, GL_UNSIGNED_BYTE, NULL);
    tex->uploadPixelData(data, internal, 0);
    return tex;
}

Texture *TextureManager::createRandomTexture(const std::string &name, int w, int h, int d, int frames) {
    Texture *tex = createTexture(name, w, h, d, frames);
    // Generate the random data.
    unsigned char *pixels = new unsigned char[w * h * d * 4];
    for (int i = 0; i < w * h * d * 4; i++) {
        pixels[i] = Math::RandI(255);
    }

    // Upload the data to OGL.
    PixelData data(GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    tex->uploadPixelData(data, GL_RGBA, 0);
    delete[] pixels;

    // Doesn't have mipmaps, so turn off filtering.
    tex->setFiltering(GL_NEAREST, GL_NEAREST);

    return tex;
}

//Texture* TextureManager::initCube(const string &name, int w, int h, int frames) {
//    Texture *tex = new Texture();
//    if (!initCube(tex, name, w, h, frames)) {
//        delete tex;
//        tex = NULL;
//    }
//
//    return tex;
//}
//
//bool TextureManager::initCube(Texture *tex, const string &name, int w, int h, int frames) {
//    GLuint *ids = initTexture(GL_TEXTURE_CUBE_MAP, frames);
//    if (ids) {
//        tex->setInternals(GL_TEXTURE_CUBE_MAP, ids, frames, w, h, 1);
//        registerResource(name, tex);
//        return true;
//    }
//
//    return false;
//}
//
//void TextureManager::blankCube(Texture *tex, GLenum internal, int level, int frame) {
//    tex->bind(0, frame);
//    GLenum format = GL_RGBA; // This is needed for ATI hardware :/
//    if (internal == GL_DEPTH_COMPONENT   || internal == GL_DEPTH_COMPONENT16 ||
//        internal == GL_DEPTH_COMPONENT24 || internal == GL_DEPTH_COMPONENT32) {
//        format = GL_DEPTH_COMPONENT;
//    }
//
//    for (int i = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; i < GL_TEXTURE_CUBE_MAP_NEGATIVE_X + 6; i++) {
//        glTexImage2D(i, 0, internal, tex->width(),
//                     tex->height(), 0, format, GL_INT, NULL);
//    }
//
//    tex->release();
//}
//
//void TextureManager::texCube(Texture *tex, PixelData &data, GLenum face, int level,
//                             int frame) {
//    tex->bind(0, frame);
//    int width  = tex->width();
//    int height = tex->height();
//    int depth  = tex->depth();
//    calcMipMapSize(level, width, height, depth);
//
//    if (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
//        data.format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
//        data.format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
//        if (level < 0) {
//            Error("No mipmaps on precompressed textures");
//            return;
//        } else {
//            glCompressedTexImage2D(face, level, data.format, width, height, 0, data.type *
//                                   ceil(width / 4) * ceil(height / 4), data.pixels);
//        }
//    } else {
//        if (level < 0) {
//            gluBuild2DMipmaps(face, data.internal, width, height, data.format, data.type,
//                              data.pixels);
//        } else {
//            glTexImage2D(face, level, data.internal, width, height, 0, data.format,
//                         data.type, data.pixels);
//        }
//    }
//
//    tex->release();
//}
