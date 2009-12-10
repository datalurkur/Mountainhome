/*
 *  Viewport.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_
#include <Vector.h>
#include <list>

class Filter;
class RenderTarget;
class RenderContext;
class RenderSource;

/*! \brief 
    \author Brent Wilson
    \date 4/6/07 */
class Viewport {
public:
    Viewport(RenderTarget *target);
    ~Viewport();

    void activate(RenderContext* context);
    RenderTarget* getTarget();

    void setRatios(Real x, Real y, Real w, Real h);
    void setPixelDimensions(int x, int y, int w, int h);
    void getPixelDimensions(int &x, int &y, int &w, int &h);

    /*! This adjusts the background color of the screen for when the scene is rendered.
        \param color The new background color. */
    void setBGColor(const Color4 &color);

private:
    /*! This takes the previously redirected output and applies the Filter. Finally it
        sends the new output to the final target.
        \sa void initFiltering(RenderContext *context);
        \param context The current RenderContext. */
    void applyFilter(RenderContext *context);
    
private:
    RenderTarget *_target;
    Real _xRatio, _yRatio;
    Real _wRatio, _hRatio;

    Color4 _clearColor;
};

#endif
