/*
 *  RenderTarget.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_
#include <Math3D.h>
#include <list>

class RenderSource;
class RenderContext;

/*! \brief Is an eventual final destination for rendered information (ex. a Window).
    \author Brent Wilson
    \date 4/6/07 */
class RenderTarget {
public:
    RenderTarget(int width, int height);
    ~RenderTarget();

    virtual void enable() = 0;
    virtual void resize(int width, int height);

    RenderSource* getRenderSource(int index);
    void addRenderSource(RenderSource *listener);

    void removeRenderSource(int index);
    void removeAllRenderSources();

    int getWidth() const;
    int getHeight() const;

protected:
    std::list<RenderSource*>::iterator getIterator(int index);
    std::list<RenderSource*> _sources;
    int _width, _height;
};

#endif
