/*
 *  SDL_Helper.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "SDL_Helper.h"
#include <Base/Assertion.h>
#include <Base/Math3D.h>
#include <Base/Logger.h>

int GetSDLGLAttribute(SDL_GLattr attr) {
    int result;
    if (SDL_GL_GetAttribute(attr, &result)) {
        THROW(InternalError, "Could not query GL attribute " << attr << ": " << SDL_GetError());
    }
    return result;
}

void FlipSDLPixels(SDL_Surface* surface) {
    unsigned char* pixels = (unsigned char*) surface->pixels;
    for (int r1 = 0; r1 < surface->h >> 1; r1++) {
        int r2 = surface->h - 1 - r1;
        for (int c = 0; c < surface->pitch; c++) {
            Math::Swap(pixels[(r1 * surface->pitch) + c], pixels[(r2 * surface->pitch) + c]);
        }
    }
}

void SDL_DrawPixel(SDL_Surface *sdlScreen, Uint32 x, Uint32 y, Uint8 R, Uint8 G, Uint8 B) {
    if (x > sdlScreen->w || y > sdlScreen->h) {
        Error("Pixel location out of bounds: " << x << y);
        return;
    }
    
    Uint32 color = SDL_MapRGB(sdlScreen->format, R, G, B);
    
    if (SDL_MUSTLOCK(sdlScreen) ) {
        if ( SDL_LockSurface(sdlScreen) < 0 ) {
            Error("Could not lock surface\n");
            return;
        }
    }
    
    switch (sdlScreen->format->BytesPerPixel) {
        case 1: { // Assuming 8-bpp
            Uint8 *bufp;
            
            bufp = (Uint8 *)sdlScreen->pixels + y * sdlScreen->pitch + x;
            *bufp = color;
        }
            break;
            
        case 2: { // Probably 15-bpp or 16-bpp
            Uint16 *bufp;
            
            bufp = (Uint16 *)sdlScreen->pixels + y * sdlScreen->pitch / 2 + x;
            *bufp = color;
        }
            break;
            
        case 3: { // Slow 24-bpp mode, usually not used
            Uint8 *bufp;
            
            bufp = (Uint8 *)sdlScreen->pixels + y * sdlScreen->pitch + x;
            *(bufp + sdlScreen->format->Rshift/8) = R;
            *(bufp + sdlScreen->format->Gshift/8) = G;
            *(bufp + sdlScreen->format->Bshift/8) = B;
        }
            break;
            
        case 4: { // Probably 32-bpp
            Uint32 *bufp;
            
            bufp = (Uint32 *)sdlScreen->pixels + y * sdlScreen->pitch / 4 + x;
            *bufp = color;
        }
            break;
    }
    
    if ( SDL_MUSTLOCK(sdlScreen) ) {
        SDL_UnlockSurface(sdlScreen);
    }
    SDL_UpdateRect(sdlScreen, x, y, 1, 1);
}

SDL_Surface* SDL_Create32BitRGBSurface(Uint32 flags, int w, int h) {
    SDL_Surface *screen;
    Uint32 rmask, gmask, bmask, amask;
    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    screen = SDL_CreateRGBSurface(flags, w, h, 32, rmask, gmask, bmask, amask);
    
    if(!screen) {
        Error("Creation of 32bit RGB surface failed: " << SDL_GetError());
    }
    
    return screen;
}

void SDL_GetPixel(const SDL_Surface *screen, Uint32 x, Uint32 y, float &R, float &G, float &B) {
    if (x > screen->w || y > screen->h) {
        Error("Pixel location out of bounds: " << x << y);
        return;
    }
    
    Uint8 r, g, b;
    Uint32 pixel = 0;
    switch(screen->format->BytesPerPixel){
        case 1: {
            Uint8 *bufp = (Uint8 *)screen->pixels + y * screen->pitch + x;
            pixel = *bufp;
        }
            break;
        case 2: {
            Uint16 *bufp = (Uint16 *)screen->pixels + y * screen->pitch / 2 + x;
            pixel = *bufp;
        }
            break;
        case 3: {
            Uint8 *bufp = (Uint8 *)screen->pixels + y * screen->pitch + x * 3;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
            pixel = bufp[0] | (bufp[1] << 8) | (bufp[2] << 16);
#else
            pixel = bufp[2] | (bufp[1] << 8) | (bufp[0] << 16);
#endif
        }
            break;
        case 4: {
            Uint32 *bufp = (Uint32 *)screen->pixels + y * screen->pitch / 4 + x;
            pixel = *bufp;
        }
            break;
    }
    SDL_GetRGB(pixel, screen->format, &r, &g, &b);
    
    R = r / 255.0f;
    G = g / 255.0f;
    B = b / 255.0f;
}


void SDL_GetPixel(SDL_Surface *surface, Uint16 x, Uint16 y, int &r, int &g, int &b, int &a) {
    SDL_PixelFormat *fmt;
    SDL_Color       *color;
    Uint8            index;
    Uint32           temp;
    Uint32           pixel = 255;
    Uint8           *img;
    
    fmt = surface->format;
    img = (Uint8 *)surface->pixels + x*fmt->BytesPerPixel+surface->pitch*y;
    
    /* Check the bitdepth of the surface */
    if( fmt->BitsPerPixel==8 ){
        /* Get the topleft pixel */
        index = *img;
        color = &(fmt->palette->colors[index]);
        a = 255;
        r = color->r;
        g = color->g;
        b = color->b;
        
    }else{
        pixel = *(Uint32*)img;

        /* Get Red component */
        temp  = pixel&fmt->Rmask;  /* Isolate red component */
        temp  = temp>>fmt->Rshift; /* Shift it down to 8-bit */
        temp  = temp<<fmt->Rloss;  /* Expand to a full 8-bit number */
        r     = (int)temp;

        /* Get Green component */
        temp  = pixel&fmt->Gmask;  /* Isolate green component */
        temp  = temp>>fmt->Gshift; /* Shift it down to 8-bit */
        temp  = temp<<fmt->Gloss;  /* Expand to a full 8-bit number */
        g     = (int)temp;

        /* Get Blue component */
        temp  = pixel&fmt->Bmask;  /* Isolate blue component */
        temp  = temp>>fmt->Bshift; /* Shift it down to 8-bit */
        temp  = temp<<fmt->Bloss;  /* Expand to a full 8-bit number */
        b     = (int)temp;

        /* Get Alpha component */
        temp  = pixel&fmt->Amask;  /* Isolate alpha component */
        temp  = temp>>fmt->Ashift; /* Shift it down to 8-bit */
        temp  = temp<<fmt->Aloss;  /* Expand to a full 8-bit number */
        a     = (int)temp;
    }
}


