/*
 *  GameState.h
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_
#include <Engine/State.h>

class RenderTarget;
class OctreeScene;
class OverlayScene;
class Material;
class Rectangle;
class Sphere;

class GameState : public State {
    static const float Speed;

public:
    GameState(RenderTarget *window);
    virtual ~GameState();

    virtual void setup(va_list args);
    virtual void teardown();

    virtual void update(int elapsed);

    virtual void keyPressed(KeyEvent *event);

private:
    RenderTarget *_mainWindow; ///\todo Should I store main window here or just call up to a parent when I need it?
    OctreeScene  *_gameScene;
    OverlayScene *_overlay;
    Sphere       *_sphere;
    Rectangle    *_rec;

    Material *_r;
    Material *_g;
    Material *_b;

    float _delta;
    bool _move;
};

#endif
