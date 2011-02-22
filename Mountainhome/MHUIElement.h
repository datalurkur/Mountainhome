/*
 *  MHUIElement.h
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <Render/Renderable.h>
#include <string>

#include "MHUIManager.h"

class MHUIManager;
class Font;

class MHUIElement {
public:
    MHUIElement();
    virtual ~MHUIElement();

// Event management.
    virtual void onMouseOver(int x, int y);

    virtual void onClick(int x, int y, int button);

// Child management.
    void deleteAllChildren();

    bool deleteChild(MHUIElement *child);

    void addChild(MHUIElement *child);

    const std::list<MHUIElement *> & getChildren();

    MHUIElement * getParent();

// Position management.
    int getX();
    void setX(int x);

    int getY();
    void setY(int y);

    bool getAlwaysOnTop();
    void setAlwaysOnTop(bool val);

// Renderable management.
    void addRenderable(Renderable *renderable);

    void deleteRenderable(Renderable *renderable);

    void deleteAllRenderables();

    void addRenderablesToList(RenderableList &list);

private:
    friend class MHUIManager; // To call updateDerivedValues.

    void updateDerivedValues();

    void setParent(MHUIElement *parent);

private:
// Position management.
    Vector2 _localPosition;
    Vector2 _absolutePosition; /*!< Used internally for rendering. */

    bool _onTop;

// Child management.
    MHUIElement *_parent;
    std::list<MHUIElement *> _children;

// Renderable management.
    RenderableList _renderables;

};

#endif

