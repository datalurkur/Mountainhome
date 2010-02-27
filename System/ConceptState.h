/*
 *  ConceptState.h
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ConceptState_H_
#define _ConceptState_H_
#include <Engine/State.h>
#include <Base/Vector.h>

class RenderTarget;
class OverlayScene;
class Material;
class Rectangle;
class Sphere;
class Scene;

class ConceptState : public State {
    static const float Speed;

public:
    ConceptState();
    virtual ~ConceptState();

    virtual void setup(va_list args);
    virtual void teardown();

    virtual void update(int elapsed);

    virtual void keyPressed(KeyEvent *event);
    virtual void keyReleased(KeyEvent *event);
    virtual void mouseMoved(MouseMotionEvent *event);

private:
    Scene  *_gameScene;
    Sphere *_sphere;

    Material *_r;
    Material *_g;
    Material *_b;

    float _delta;
    bool _moveLight;

    Camera *_activeCam;
    Camera *_lCam, *_rCam;
    Degree _yaw, _pitch;
    Vector3 _move;
};

#endif
