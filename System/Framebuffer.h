/*
 *  Framebuffer.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_
#include "RenderTarget.h"
#include "GL_Helper.h"

/*! \brief Is an offscreen RenderTarget that allows the results to be accessed like a
    Texture.
    \todo Remove GL_Helper.h
    \author Brent Wilson
    \date 4/23/07 */
class Texture;
class Framebuffer : public RenderTarget {
public:
    Framebuffer(int width, int height, GLenum colorMode = GL_RGBA,
                bool useDepth = true, bool useStencil = false);
    ~Framebuffer();
    
    bool isDepthBuffer();
    bool isColorBuffer();
    Texture* getTexture();

    void disable();
    virtual void enable();
    void bindTexture(int level = 0);

protected:
    GLuint _fb;
    GLuint _depthRb;
    GLuint _stencilRb;
    Texture *_fbTexture;
    GLenum _mode;

    void initFramebuffer();
    void initDepthRenderbuffer();
    void initStencilRenderbuffer();
    void initTexture(int width, int height);
};

#endif
