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

#include "ResourceGroupManager.h"
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
        GLenum layout;
        if (surface->format->BitsPerPixel == 24) { layout = GL_BGR; }
        else if(surface->format->BitsPerPixel == 32) { layout = GL_BGRA; }
        else {
            SDL_FreeSurface(surface);
            Error("TextureManager: Unknown format");
            return NULL;
        }

        data->setPixelData((unsigned char*)surface->pixels, layout, surface->w, surface->h, false);
    }

    FlipSDLPixels(surface);
    return surface;
}

TextureSDL::Factory::Factory(ResourceGroupManager *manager, TextureManager *tManager):
ResourceFactory<Texture>(manager, false), _textureManager(tManager) {}
TextureSDL::Factory::~Factory() {}

bool TextureSDL::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    return ext == "bmp" || ext == "pnm" || ext == "xpm" || ext == "lbm" || ext == "gif" ||
           ext == "gif" || ext == "png" || ext == "tga" || ext == "jpg" || ext == "jpeg"||
           ext == "tiff";
}

Texture *TextureSDL::Factory::load(const std::string &name) {
    std::string fullName = _resourceGroupManager->findResource(name);

    PixelData data;
    SDL_Surface *surface = readTextureSDL(fullName, &data);

    Texture *result = NULL;
    if (surface) {
        result = _textureManager->createTexture(name);
        result->uploadPixelData(data, GL_RGBA);
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
