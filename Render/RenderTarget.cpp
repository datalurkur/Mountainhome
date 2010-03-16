/*
 *  RenderTarget.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "RenderContext.h"
#include "RenderTarget.h"
#include "RenderSource.h"
#include "Viewport.h"

RenderTarget::RenderTarget(int width, int height): _clearColor(0, 0, 0, 1),
_width(width), _height(height) {}

RenderTarget::~RenderTarget() {
    removeAllViewports();
}

void RenderTarget::resize(int width, int height) {
    _width = width;
    _height = height;

    ViewportIterator itr = _viewports.begin();
    for(; itr != _viewports.end(); itr++) {
        itr->second->resize(width, height);
    }
}


void RenderTarget::setBGColor(const Color4 &color) {
    _clearColor = color;
}

void RenderTarget::render(RenderContext* context) {
    enable();
    context->clearBuffers(_clearColor);

    ViewportIterator itr = _viewports.begin();
    for(; itr != _viewports.end(); itr++) {
        //Info("Rendering z level " << itr->first);
        itr->second->render(context);
    }
}

Viewport* RenderTarget::addViewport(RenderSource *source, int zLevel, Real x, Real y, Real w, Real h) {
    if (_viewports.find(zLevel) != _viewports.end()) {
        THROW(DuplicateItemError, "Viewport already exists at z level " << zLevel << "!");
    }

    Viewport *v = new Viewport(source, this);
    v->setRatios(x, y, w, h);
    _viewports[zLevel] = v;
    return v;
}

void RenderTarget::removeAllViewports() {
    clear_map(_viewports);
}

int RenderTarget::getWidth() const {
    return _width;
}

int RenderTarget::getHeight() const {
    return _height;
}
