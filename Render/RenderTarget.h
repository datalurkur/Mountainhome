/*
 *  RenderTarget.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_
#include <Base/Math3D.h>
#include <Base/Vector.h>
#include <list>

class RenderSource;
class RenderContext;
class Viewport;

/*! \brief Is an eventual final destination for rendered information (ex. a Window).
    \author Brent Wilson
    \date 4/6/07 */
class RenderTarget {
public:
    typedef std::map<int, Viewport*, std::less<int> > ViewportMap;
    typedef ViewportMap::iterator ViewportIterator;

public:
    RenderTarget(int width, int height);
    virtual ~RenderTarget();

    virtual void enable() = 0;
    virtual void resize(int width, int height);

    /*! This adjusts the background color of the screen for when the scene is rendered.
        \param color The new background color. */
    void setBGColor(const Color4 &color);

    void render(RenderContext* context);

    Viewport* addViewport(int zLevel = 0, Real x = 0.0f, Real y = 0.0f, Real w = 1.0f, Real h = 1.0f);
    const ViewportMap& getViewports();
    void removeAllViewports();

    int getWidth() const;
    int getHeight() const;


protected:
    Color4 _clearColor;
    ViewportMap _viewports;
    int _width, _height;
};

#endif
