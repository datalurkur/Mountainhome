/*
 *  Window.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Base/Exception.h>
#include <Render/SDL_Helper.h>
#include <Render/GL_Helper.h>
#include "Window.h"

Window::Window(int width, int height, const std::string &caption):
    _width(width),
    _height(height),
    _videoFlags(0),
    _framebuffer(NULL),
    _caption("Engine"),
    _iconPath(""),
    _postCaption("")
{
    initSDL();
    setCaption(caption, "");
    resize(width, height);
}

Window::Window(const std::string &caption):
    _width(-1),
    _height(-1),
    _videoFlags(0),
    _framebuffer(NULL),
    _caption("Engine"),
    _iconPath(""),
    _postCaption("")
{
    initSDL();
    setCaption(caption, "");
}

Window::~Window() {
    if (_framebuffer) {
        SDL_FreeSurface((SDL_Surface*)_framebuffer);
    }

    SDL_Quit();
}

int Window::getWidth() {
    return _width;
}

int Window::getHeight() {
    return _height;
}

void Window::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        THROW(InvalidStateError, "SDL initialization failed: " << SDL_GetError());
    }

    _videoFlags  = SDL_OPENGL;          // Enable OpenGL in SDL
    _videoFlags |= SDL_GL_DOUBLEBUFFER; // Enable double buffering
    _videoFlags |= SDL_HWPALETTE;       // Store the palette in hardware
    _videoFlags |= SDL_HWSURFACE;       // Use HW surfaces if possible
    _videoFlags |= SDL_HWACCEL;         // Use HW accelerated blits if possible

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Take control of the mouse and hide the cursor.
    // This is needed for reasonable mouse look.
    SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_GrabInput(SDL_GRAB_ON);
}

void Window::rebuild(int width, int height, int aasamples, bool fullscreen, bool vsync) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, aasamples > 0 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aasamples);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vsync);
    if (fullscreen) { _videoFlags ^= SDL_FULLSCREEN; }
    resize(width, height);

    printVideoInfo();
}

void Window::printVideoInfo() {
    const SDL_VideoInfo *videoInfo;
    if (!(videoInfo = SDL_GetVideoInfo())) {
        THROW(InternalError, "Video query failed: " << SDL_GetError());
    }

    InfoC(LogStream::GraphicsChannel, "Video state information:");
    LogStream::IncrementIndent();
    InfoC(LogStream::GraphicsChannel, "SDL accel:     " << videoInfo->hw_available);
    InfoC(LogStream::GraphicsChannel, "OpenGL accel:  " << GetSDLGLAttribute(SDL_GL_ACCELERATED_VISUAL));
    InfoC(LogStream::GraphicsChannel, "Resolution:    " << videoInfo->current_w << "x" << videoInfo->current_h);
    InfoC(LogStream::GraphicsChannel, "FB size:       " << GetSDLGLAttribute(SDL_GL_BUFFER_SIZE));
    InfoC(LogStream::GraphicsChannel, "Depth size:    " << GetSDLGLAttribute(SDL_GL_DEPTH_SIZE));
    InfoC(LogStream::GraphicsChannel, "Doublebuffer:  " << GetSDLGLAttribute(SDL_GL_DOUBLEBUFFER));
    InfoC(LogStream::GraphicsChannel, "Vertical sync: " << GetSDLGLAttribute(SDL_GL_SWAP_CONTROL));
    InfoC(LogStream::GraphicsChannel, "FSAA enabled:  " << GetSDLGLAttribute(SDL_GL_MULTISAMPLEBUFFERS));
    InfoC(LogStream::GraphicsChannel, "FSAA samples:  " << GetSDLGLAttribute(SDL_GL_MULTISAMPLESAMPLES));
    LogStream::DecrementIndent();
}

void Window::resize(int width, int height) {
    if (_framebuffer) {
        SDL_FreeSurface((SDL_Surface*)_framebuffer);
    }

    if (!(_framebuffer = SDL_SetVideoMode(width, height, 32, _videoFlags))) {
        THROW(InternalError, "Window: Video mode set failed: " << SDL_GetError());
    }

    _width = width;
    _height = height;
}

void Window::updateCaption() const {
    std::string text = _caption;
    if (_postCaption.size() > 0) {
        text += " (" + _postCaption + ")";
    }

    SDL_WM_SetCaption(text.c_str(), _iconPath.c_str());
}

void Window::setPostCaption(const std::string &caption) {
    _postCaption = caption;
    updateCaption();
}

void Window::setCaption(const std::string &caption, const std::string &iconPath) {
    _caption = caption;
    _iconPath = iconPath;
    updateCaption();
}

void Window::swapBuffers() const {
    SDL_GL_SwapBuffers();
}


