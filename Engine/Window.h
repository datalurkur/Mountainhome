/*
 *  Window.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "RenderTarget.h"
#include <string>

/*! \brief Is a RenderTarget that represents an actual onscreen window. */
class Window : public RenderTarget {
public:
    Window(const std::string &caption);
    Window(int width, int height, const std::string &caption);
    virtual ~Window();

    void rebuild(int width, int height, int aasamples, bool fullscreen);

    void setCaption(const std::string &caption, const std::string &iconPath);
    void setPostCaption(const std::string &caption);
    void setSampleCount(int samples);
    void toggleFullscreen();

    virtual void enable();
    virtual void resize(int width, int height);

    /*! Swaps the front and back buffers, drawing what we've rendered so far. */
    void swapBuffers() const;

    /*! Prints out notable video context information. */
    void printVideoInfo();

private:
    void initSDL();
    void updateFPS(int elapsedTime) const;
    void updateCaption() const;

    int _videoFlags;
    void *_framebuffer;

    std::string _caption;
    std::string _iconPath;
    std::string _postCaption;

};

#endif
