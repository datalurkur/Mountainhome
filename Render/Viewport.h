/*
 *  Viewport.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_
#include <Base/Vector.h>
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
    Viewport(RenderSource *source, RenderTarget *target);
    virtual ~Viewport();

    void render(RenderContext* context);
    void resize(int newParentWidth, int newParentHeight);

    RenderTarget* getTarget();

    RenderSource* getSource(int index);
    RenderSource* addSource(RenderSource* source, int zLevel);

    void setRatios(Real x, Real y, Real w, Real h);
    void setPixelDimensions(int x, int y, int w, int h);
    void getPixelDimensions(int &x, int &y, int &w, int &h);

private:
    /*! This takes the previously redirected output and applies the Filter. Finally it
        sends the new output to the final target.
        \sa void initFiltering(RenderContext *context);
        \param context The current RenderContext. */
    void applyFilter(RenderContext *context);
    
private:

    typedef std::map<int, RenderSource*, std::less<int> > SourceMap;

    SourceMap _sources;

//    RenderSource *_source;
    RenderTarget *_target;

    Real _xRatio, _yRatio;
    Real _wRatio, _hRatio;
};

#endif