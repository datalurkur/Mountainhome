/*
 *  Framebuffer.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_
#include "GL_Helper.h"

/*!\brief Is an offscreen RenderTarget that allows the results to be accessed like a Texture.
 * \todo Remove GL_Helper.h */
class Texture;
class Framebuffer {
public:
    Framebuffer(Texture *target, bool useDepth = true, bool useStencil = false);
    virtual ~Framebuffer();

    int getWidth();
    int getHeight();

    bool isDepthBuffer();
    bool isColorBuffer();
    Texture* getTexture();

    void enable();
    void disable();

protected:
    GLuint _fb;
    GLuint _depthRb;
    GLuint _stencilRb;
    Texture *_fbTexture;
    GLenum _mode;

    void initFramebuffer();
    void initDepthRenderbuffer();
    void initStencilRenderbuffer();

};

#endif
