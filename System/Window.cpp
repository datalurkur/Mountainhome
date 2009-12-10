/*
 *  Window.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Window.h"
#include "GL_Helper.h"
#include "SDL_Helper.h"

#include <Log.h>

Window::Window(int width, int height, bool fullscreen, const std::string &caption)
:RenderTarget(width, height), _videoFlags(0), _framebuffer(NULL), _caption("Engine"),
_iconPath(""), _postCaption("") {
    initSDL(width, height);
    setCaption(caption, "");

    if (fullscreen) {
        toggleFullscreen();
    }
}

Window::~Window() {
    if (_framebuffer) {
        SDL_FreeSurface((SDL_Surface*)_framebuffer);
    }

    SDL_Quit();
}

void Window::enable() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

bool Window::initSDL(int width, int height) {
    const SDL_VideoInfo *videoInfo;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        Error("SDL initialization failed:" << SDL_GetError());
        return false;
    }

    if (!(videoInfo = SDL_GetVideoInfo())) {
        Error("Video query failed:" << SDL_GetError());
        return false;
    }

    _videoFlags  = SDL_OPENGL;          // Enable OpenGL in SDL
//    _videoFlags |= SDL_OPENGLBLIT;    // Allow blitting in openGL
    _videoFlags |= SDL_GL_DOUBLEBUFFER; // Enable double buffering
    _videoFlags |= SDL_HWPALETTE;       // Store the palette in hardware
//    _videoFlags |= SDL_RESIZABLE;     // Enable window resizing
    
    if (videoInfo->hw_available) { _videoFlags |= SDL_HWSURFACE; }
    else { _videoFlags |= SDL_SWSURFACE; }
    
    if (videoInfo->blit_hw) { _videoFlags |= SDL_HWACCEL; }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    resize(width, height);

    return true;
}

void Window::setSampleCount(int samples) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, samples);
    resize(_width, _height);
}

void Window::resize(int width, int height) {
    if (_framebuffer) {
        SDL_FreeSurface((SDL_Surface*)_framebuffer);
    }

    if (!(_framebuffer = SDL_SetVideoMode(width, height, 32, _videoFlags))) {
        Error("Window: Video mode set failed:" << SDL_GetError());
    }

    RenderTarget::resize(width, height);
}

void Window::toggleFullscreen() {
    if (SDL_WM_ToggleFullScreen((SDL_Surface*)_framebuffer) != 1) {
        Error("Unable to toggle fullscreen:" << SDL_GetError());
    }
}

void Window::updateCaption() const {
    std::string text = _caption + " (" + _postCaption + ")";
    SDL_WM_SetCaption(text.c_str(), _iconPath.c_str());
}

void Window::setPostCaption(const std::string &caption) {
    _postCaption = caption;
    updateCaption();
}

void Window::setCaption(const string &caption, const string &iconPath) {
    _caption = caption;
    _iconPath = iconPath;
    updateCaption();
}

void Window::swapBuffers() const {
    SDL_GL_SwapBuffers();
}

