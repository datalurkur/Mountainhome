/*
 *  AbstractCore.h
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ABSTRACTCORE_H_
#define _ABSTRACTCORE_H_
#include "WindowListener.h"
#include "ParentState.h"

#include <list>

class Window;
class EventPump;
class RenderContext;
class FrameListener;
class Camera;

class AbstractCore : public ParentState, WindowListener {
public:
    AbstractCore();
    AbstractCore(int width, int height, bool fullscreen, const std::string &caption);
    virtual ~AbstractCore();

    Window* getMainWindow();
    EventPump* getEventPump();
    RenderContext* getRenderContext();

    virtual void innerLoop(int elapsed) = 0;
    virtual void startMainLoop();
    virtual void stopMainLoop();

    void addFrameListener(FrameListener *listener);

public:
    virtual void keyTyped(KeyEvent *event) { }
    virtual void keyPressed(KeyEvent *event) { }
    virtual void keyReleased(KeyEvent *event) { }

    virtual void mouseClicked(MouseButtonEvent *event) { }
    virtual void mousePressed(MouseButtonEvent *event) { }
    virtual void mouseReleased(MouseButtonEvent *event) { }
    virtual void mouseMoved(MouseMotionEvent *event) { }

protected:
    void calculateFramerate(int elapsed);
    bool broadcastFrameEvent(int elapsed);

    virtual void windowResized(int w, int h);
    virtual void windowClosing();

    int getTime();
    void setPostText();

protected:
    bool _running;
    Real _framerate;

    Window *_mainWindow;
    RenderContext *_renderContext;
    EventPump *_eventPump;

    std::list<FrameListener*> _frameListeners;
};

#endif
