/*
 *  TextureSDL.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 *  TextureSDL is an image Factory wrapper class for Texture
 *
 */

#include "SDL_Helper.h"
#include "TextureSDL.h"
#include "TextureManager.h"
#include "Texture.h"
#include "PixelData.h"

#include <Base/ResourceGroupManager.h>
#include <Base/FileSystem.h>
#include <Base/Assertion.h>
#include <Base/Logger.h>

SDL_Surface *readTextureSDL(const std::string &name, PixelData *data) {
    SDL_Surface *surface;
    std::string fullName;
    if (!FileSystem::GetReadName(name, fullName)) {
        Error("TextureManager: Could not find the given file.");
        Error("  Given name : " << name);
        Error("  Search name: " << fullName);
        return NULL;
    }

    if (!(surface = IMG_Load(fullName.c_str()))) {
        Error("TextureManager: Could not open: " << fullName);
        return NULL;
    }
    
    if (surface->format->palette) {
        //TODO add support for loading palette images. It could be neat.
        Error("TextureManager: " << fullName << " is an indexed image");
        SDL_FreeSurface(surface);
        return NULL;
    }

    if (data) {
        data->pixels = surface->pixels;
        data->type = GL_UNSIGNED_BYTE;

        if (surface->format->BitsPerPixel == 24) { data->format = GL_RGB; }
        else if(surface->format->BitsPerPixel == 32) { data->format = GL_RGBA; }
        else {
            SDL_FreeSurface(surface);
            Error("TextureManager: Unknown format");
            return NULL;
        }

        data->internal = data->format;
    }

    return surface;
}

TextureSDL::Factory::Factory() {}
TextureSDL::Factory::~Factory() {}

bool TextureSDL::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    return ext == "bmp" || ext == "pnm" || ext == "xpm" || ext == "lbm" || ext == "gif" ||
           ext == "gif" || ext == "png" || ext == "tga" || ext == "jpg" || ext == "jpeg"||
           ext == "tiff";
}

Texture *TextureSDL::Factory::load(const std::string &name) {
    std::string fullName = ResourceGroupManager::Get()->findResource(name);

    PixelData data;
    SDL_Surface *surface = readTextureSDL(fullName, &data);
    FlipSDLPixels(surface);

    Texture *result = NULL;
    if (surface) {
        result = TextureManager::Get()->createTexture(name, surface->w, surface->h, 1);
        result->uploadPixelData(data, -1);
        SDL_FreeSurface(surface);
    }

    return result;
}

//Texture* TextureSDL::loadCubeMap(const std::string &name,
//                                 const std::string files[6]) {
//    if (!canLoad(name)) {
//        Error("Texture format unsupported for cube maps: " << name);
//        return NULL;
//    }
//
//    PixelData data;
//    Texture *result = NULL;
//
//    for (int i = 0; i < 6; i++) {
//        SDL_Surface *surface = readTextureSDL(files[i], &data);
//        if (result = NULL) {
//            result = TextureManager::Get()->initCube(name, surface->w, surface->h);        
//        }
//
//        TextureManager::Get()->texCube(result, data, i + GL_TEXTURE_CUBE_MAP_NEGATIVE_X, -1, 1);
//        SDL_FreeSurface(surface);
//    }
//
//    return result;
//}
//
//Texture* TextureSDL::loadAnimated(const std::string &name, int n,
//                                  const std::string *files) {
//    if (!canLoad(name)) {
//        Error("Texture format unsupported for animated textures: " << name);
//        return NULL;
//    }
//
//    PixelData data;
//    Texture *result = NULL;
//
//    for (int i = 0; i < 6; i++) {
//        SDL_Surface *surface = readTextureSDL(files[i], &data);
//        if (result = NULL) {
//            result = TextureManager::Get()->init2D(name, n, surface->w, surface->h);
//        }
//
//        TextureManager::Get()->tex2D(result, data, -1, n);
//        SDL_FreeSurface(surface);
//    }
//
//    return result;
//}
//
