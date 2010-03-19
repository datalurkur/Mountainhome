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

Framebuffer::Framebuffer(int width, int height, GLenum colorMode, bool useDepth, bool useStencil)
:RenderTarget(width, height), _fb(0), _depthRb(0), _stencilRb(0), _fbTexture(NULL), _mode(colorMode) {
    initTexture(width, height);
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

void Framebuffer::initTexture(int width, int height) {
    _fbTexture = TextureManager::Get()->createBlankTexture("", _mode, width, height);
    _fbTexture->setTexCoordHandling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _fbTexture->setFiltering(GL_NEAREST, GL_NEAREST);
}

void Framebuffer::initFramebuffer() {
    glGenFramebuffersEXT(1, &_fb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
    GLenum attach = isDepthBuffer() ? GL_DEPTH_ATTACHMENT_EXT : GL_COLOR_ATTACHMENT0_EXT;
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, _fbTexture->id(), 0);
    if (isDepthBuffer()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    CheckFramebufferStatus();
}

void Framebuffer::initDepthRenderbuffer() {
    glGenRenderbuffersEXT(1, &_depthRb);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _depthRb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _fbTexture->width(), _fbTexture->height());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depthRb);
    CheckFramebufferStatus();
}

void Framebuffer::initStencilRenderbuffer() {
    glGenRenderbuffersEXT(1, &_stencilRb);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _stencilRb);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, _fbTexture->width(), _fbTexture->height());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _stencilRb);
    CheckFramebufferStatus();
}

bool Framebuffer::isDepthBuffer() {
    return _mode == GL_DEPTH_COMPONENT32 ||
           _mode == GL_DEPTH_COMPONENT24 ||
           _mode == GL_DEPTH_COMPONENT16 ||
           _mode == GL_DEPTH_COMPONENT;
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

void Framebuffer::disable() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void Framebuffer::bindTexture(int level) {
    _fbTexture->bind(level);
}

Texture* Framebuffer::getTexture() {
    return _fbTexture;
}
