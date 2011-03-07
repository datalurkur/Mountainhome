/*
 *  Framebuffer.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Framebuffer.h"
#include "TextureManager.h"
#include "Texture.h"

Framebuffer::Framebuffer(
    Texture *target,
    bool useDepth,
    bool useStencil
):
    _fb(0),
    _depthRb(0),
    _stencilRb(0),
    _fbTexture(target)
{
    _fbTexture->setTexCoordHandling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _fbTexture->setFiltering(GL_NEAREST, GL_NEAREST);
    initFramebuffer();

    if (useDepth && !isDepthBuffer())   { initDepthRenderbuffer(); }
    if (useStencil && !isDepthBuffer()) { initStencilRenderbuffer(); }
}

Framebuffer::~Framebuffer() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteFramebuffersEXT(1, &_fb);
    glDeleteRenderbuffersEXT(1, &_depthRb);
    glDeleteRenderbuffersEXT(1, &_stencilRb);
}

int Framebuffer::getWidth() {
    return _fbTexture->getWidth();
}

int Framebuffer::getHeight() {
    return _fbTexture->getHeight();
}

void Framebuffer::initFramebuffer() {
    glGenFramebuffersEXT(1, &_fb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
    GLenum attach = isDepthBuffer() ? GL_DEPTH_ATTACHMENT_EXT : GL_COLOR_ATTACHMENT0_EXT;
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, _fbTexture->getID(), 0);
    if (isDepthBuffer()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    CheckFramebufferStatus();
}

void Framebuffer::initDepthRenderbuffer() {
    glGenRenderbuffersEXT(1, &_depthRb);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _depthRb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _fbTexture->getWidth(), _fbTexture->getHeight());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depthRb);
    CheckFramebufferStatus();
}

void Framebuffer::initStencilRenderbuffer() {
    glGenRenderbuffersEXT(1, &_stencilRb);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _stencilRb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, _fbTexture->getWidth(), _fbTexture->getHeight());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _stencilRb);
    CheckFramebufferStatus();
}

bool Framebuffer::isDepthBuffer() {
    GLuint mode = _fbTexture->getFormat();
    return mode == GL_DEPTH_COMPONENT32 ||
           mode == GL_DEPTH_COMPONENT24 ||
           mode == GL_DEPTH_COMPONENT16 ||
           mode == GL_DEPTH_COMPONENT;
}

bool Framebuffer::isColorBuffer() {
    return !isDepthBuffer();
}

void Framebuffer::enable(){
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
    if (isDepthBuffer()) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    } else {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

Texture* Framebuffer::getTexture() {
    return _fbTexture;
}
