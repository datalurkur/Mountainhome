/*
 *  RenderSource.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/25/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RENDERSOURCE_H_
#define _RENDERSOURCE_H_

class RenderContext;

/*! \brief Abstracts away the type that will dictate what is to be rendered.
    \todo Render sources (cameras) should be able to be attached to multiple viewports.
    \author Brent Wilson
    \date 4/25/07 */
class RenderSource {
public:
    RenderSource() {}
    virtual ~RenderSource() {}
    virtual void resize(int width, int height) = 0;
    virtual void render(RenderContext *context) = 0;
};

#endif
