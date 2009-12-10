/*
 *  Window.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "RenderTarget.h"
#include <string>

/*! \brief Is a RenderTarget that represents an actual onscreen window.
    \todo Remove SDL_Helper.h
    \author Brent Wilson
    \date 4/4/07 */
class Window : public RenderTarget {
public:
    Window(int width, int height, bool fullscreen, const std::string &caption);
    ~Window();

    void setCaption(const std::string &caption, const std::string &iconPath);
    void setPostCaption(const std::string &caption);
    void setSampleCount(int samples);
    void toggleFullscreen();

    virtual void enable();
    virtual void resize(int width, int height);

    void swapBuffers() const;

private:
    bool initSDL(int width, int height);
    void updateFPS(int elapsedTime) const;
    void updateCaption() const;

    int _videoFlags;
    void *_framebuffer;

    std::string _caption;
    std::string _iconPath;
    std::string _postCaption;
};

#endif
