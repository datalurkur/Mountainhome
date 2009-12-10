/*
 *  RenderSource.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/25/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RENDERSOURCE_H_
#define _RENDERSOURCE_H_

class Viewport;
class RenderContext;

/*! \brief Abstracts away the type the will dictate what is to be rendered.
    \author Brent Wilson
    \date 4/25/07 */
class RenderSource {
public:
    virtual void resize(int width, int height) = 0;
    virtual void setViewport(Viewport* viewport) = 0;
    virtual Viewport* getViewport() const = 0;
};

#endif
