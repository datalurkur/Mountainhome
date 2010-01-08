/*
 *  SDL_Helper.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SDL_HELPER_H_
#define _SDL_HELPER_H_

#if SYS_COMPILER == COMPILER_MSVC
#   pragma comment(lib, "SDLmain.lib")
#   pragma comment(lib, "SDL.lib")
#   pragma comment(lib, "SDL_image.lib")
#   pragma comment(lib, "SDL_mixer.lib")
#   pragma comment(lib, "SDL_ttf.lib")
#endif

#include <SDL/SDL.h>
#if SYS_PLATFORM == PLATFORM_APPLE
#   include <SDL_image/SDL_image.h>
#   include <SDL_mixer/SDL_mixer.h>
#   include <SDL_ttf/SDL_ttf.h>
#else
#   include <SDL/SDL_image.h>
#   include <SDL/SDL_mixer.h>
#   include <SDL/SDL_ttf.h>
#endif

void FlipSDLPixels(SDL_Surface* surface);
SDL_Surface* SDL_Create32BitRGBSurface(Uint32 flags, int w, int h);
void SDL_GetPixel(const SDL_Surface *screen, Uint32 x, Uint32 y, float &R, float &G, float &B);
void SDL_GetPixel(SDL_Surface *surface, Uint16 x, Uint16 y, int &r, int &g, int &b, int &a);
void SDL_DrawPixel(SDL_Surface *sdlScreen, Uint32 x, Uint32 y, Uint8 R, Uint8 G, Uint8 B);

#endif
