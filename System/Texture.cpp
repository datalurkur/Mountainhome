/*
 *  Texture.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Texture.h"
#include "TextureManager.h"
#include <Assertion.h>
#include <Math3D.h>

TextureManager* Texture::GetManager() {
    return TextureManager::GetSingleton();
}

#ifdef USEPNGLIB
#   include <png.h>
#endif
void Texture::SaveTexture(unsigned char *pixels, int width, int height, const std::string &name) {
#ifdef USEPNGLIB
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;

    if (!(fp = fopen(name.c_str(), "wb"))) {
        Error("Error writing out to: " << name);
        return;
    }

    if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            png_voidp_NULL,
                                            NULL, NULL))) {
        fclose(fp);
        Error("Error creating write struct");
        return;
    }

    if (!(info_ptr = png_create_info_struct(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        Error("Error creating info struct");
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        Error("Error setting up error handling");
        return;
    }

    // Init with use of streams.
    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytep* row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_bytep));
    for (int i = 0; i < height; i++) {
        row_pointers[i] = &(pixels[i * width * 4]);
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, 0, png_voidp_NULL);

    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
#endif
}

Texture* Texture::Load(const std::string &name) {
    return TextureManager::GetSingleton()->loadResource(name);
}

Texture* Texture::RandomTexture(int width, int height) {
    char buffer[64];
    snprintf(buffer, 64, "__randomTexture%ix%i", width, height);

    Texture *texture = TextureManager::GetSingleton()->getCachedResource(buffer);
    if (texture) {
        return texture;
    }

    unsigned char *pixels = new unsigned char[width * height * 4];
    for (int i = 0; i < width * height * 4; i++) {
        pixels[i] = Math::RandI(255);
    }

    TextureManager::PixelData data(GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    texture = TextureManager::GetSingleton()->init2D(buffer, width, height);
    TextureManager::GetSingleton()->tex2D(texture, data, 0);

    // Doesn't have mipmaps, so turn off filtering.
    texture->setFiltering(GL_NEAREST, GL_NEAREST);
    delete[] pixels;
    return texture;
}

Texture* Texture::Load(int width, int height, GLenum format, GLenum type, void* pixels) {
    TextureManager::PixelData data(format, type, pixels);
    Texture* texture = TextureManager::GetSingleton()->init2D("", width, height);
    TextureManager::GetSingleton()->tex2D(texture, data);
    return texture;
}

Texture::Texture() {
    setInternals(0, 0, 1, -1, -1, -1);
}

Texture::Texture(GLenum target, GLuint *ids, int frames, int w, int h, int d) {
    setInternals(target, ids, frames, w, h, d);
}

Texture::Texture(GLenum target, GLuint id, int w, int h, int d) {
    setInternals(target, id, w, h, d);
}

Texture::~Texture() {
    if (_textureId) {
        glDeleteTextures(_numFrames, _textureId);
        delete[] _textureId;
    }
}

void Texture::setInternals(GLenum target, GLuint *id, int frames, int w, int h, int d) {
    _target = target;
    _textureId = id;
    _numFrames = frames;
    _width = w;
    _height = h;
    _depth = d;
}

void Texture::setInternals(GLenum target, GLuint id, int w, int h, int d) {
    _textureId = new GLuint;
    _textureId[0] = id;

    _target = target;
    _numFrames = 1;
    _width = w;
    _height = h;
    _depth = d;

    ASSERT(_width > 0);
    ASSERT(_height > 0);
    ASSERT(_depth > 0);
    ASSERT(_numFrames > 0);
}

void Texture::bind(int level, int frame) {
    glActiveTextureARB(GL_TEXTURE0_ARB + level);
    glBindTexture(_target, _textureId[frame]);
}

void Texture::release(int level) {
    glActiveTextureARB(GL_TEXTURE0_ARB + level);
    glBindTexture(_target, 0);
}

void Texture::bindAndEnable(int level, int frame) {
    bind(level);
    glEnable(_target);
}

void Texture::releaseAndDisable(int level) {
    release(level);
    glDisable(_target);
}

void Texture::setEnvironment(GLenum env) {
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, env);
    }
}

void Texture::setFiltering(GLenum minFilter, GLenum magFilter) {
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }
}

void Texture::setTexCoordHandling(GLenum sCoord, GLenum tCoord, GLenum rCoord) {
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sCoord);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tCoord);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, rCoord);
    }
}

void Texture::setAnisoLevel(int level) {
    float largest;

    // Makesure the given aniso level isn't too large. If it is, cap it and make a note.
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
    if (level > largest) {
        Warn("Aniso level of" << level << "is to large. Max is" << largest << ".");
        level = largest;
    }

    // Set the aniso level.
    for (int i = 0; i < _numFrames; i++) {
        glBindTexture(_target, _textureId[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
    }
}

int Texture::width() { return _width; }
int Texture::height() { return _height; }
int Texture::depth() { return _depth; }

GLenum Texture::target() { return _target; }
GLuint Texture::id(int frame) {
    ASSERT(frame < _numFrames);
    return _textureId[frame];
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

