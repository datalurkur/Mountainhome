/*
 *  OverlayScene.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _OVERLAYSCENE_H_
#define _OVERLAYSCENE_H_
#include "RenderSource.h"
#include "Scene.h"

class Entity;
class RenderContext;

///\todo Perhaps this should inherit from node or something?

class OverlayScene : public Scene, public RenderSource {
public:
    OverlayScene()              { /*!\todo implement me */ }
    virtual ~OverlayScene()     { /*!\todo implement me */ }
    void attach(Entity *entity) { /*!\todo implement me */ }
    void clearScene()           { /*!\todo implement me */ }

    virtual void resize(int width, int height)  { /*!\todo implement me */ }
    virtual void render(RenderContext *context) { /*!\todo implement me */ }

};

#endif
