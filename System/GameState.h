/*
 *  GameState.h
 *  System
 *
 *  Created by loch on 2/25/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _CONCEPTSTATE_H_
#define _CONCEPTSTATE_H_
#include "RubyStateProxy.h"
#include <Base/Vector.h>

class MHWorld;

/*! GameState represents the C++ side of the main gameplay state. This exists to avoid
 *  the need to write bindings for much of the scene, camera, and input code. */
class GameState : public RubyStateProxy {
public:
#pragma mark GameState ruby bindings
    /*! Creates the ruby land State class, which maps directly to a C++ RubyStateProxy
     *  object. It also sets up the various interned values needed by the RubyStateProxy
     *  objects to call down into their paired ruby objects. */
    static void SetupBindings();

    /*! Creates a matching c++ object and stores it in a static map for later lookup.
     * \param self The ruby object we're working on. */
    static VALUE Initialize(VALUE self);

    /*! Sets the world for the game state, initializing the scene with the world. */
    static VALUE SetWorld(VALUE self, VALUE world);

    /*! Gets the world associated with the game state */
    static VALUE GetWorld(VALUE self);

public:
#pragma mark GameState declarations
    /*! Creates a new game state and associates it with the given ruby object.
     * \param robj The VALUE representing the matching ruby State object. */
    GameState(VALUE robj);

    /*! Typical d'tor */
    virtual ~GameState();

    /*! Handles actually updating stuff based on input and deferes to ruby. */
    virtual void update(int elapsed);

    /*! Handles key input, allowing the user to move the camera in the scene around. */
    virtual void keyPressed(KeyEvent *event);

    /*! Handles key input, allowing the user to move the camera in the scene around. */
    virtual void keyReleased(KeyEvent *event);

    /*! Handles mousemovement, allowing the user to look around the scene. */
    virtual void mouseMoved(MouseMotionEvent *event);

private:
    MHWorld *_world;       /*!< The world the game takes place in. */
    Degree _yaw, _pitch;   /*!< Describes the camera's current rotation. */
    Vector3 _move;         /*!< Describes the camera's current movement. */

};

#endif
