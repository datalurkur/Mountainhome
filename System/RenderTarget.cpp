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

RenderTarget::RenderTarget(int width, int height): _width(width), _height(height) {}
RenderTarget::~RenderTarget() {
    removeAllViewports();
}

void RenderTarget::resize(int width, int height) {
    _width = width;
    _height = height;

    ViewportIterator itr = _sources.begin();
    for(; itr != _sources.end(); itr++) {
        itr->second->getSource()->resize(width, height);
    }
}

void RenderTarget::render(RenderContext* context) {
    enable();

    ViewportIterator itr = _sources.begin();
    for(; itr != _sources.end(); itr++) {
        itr->second->render(context);
    }
}

Viewport* RenderTarget::addViewport(RenderSource *source, int zLevel, Real x, Real y, Real w, Real h) {
    if (_sources.find(zLevel) != _sources.end()) {
        RAISE(DuplicateItemError, "Viewport already exists at z level " << zLevel << "!");
    }

    Viewport *v = new Viewport(source, this);
    v->setRatios(x, y, w, h);
    _sources[zLevel] = v;
    return v;
}

void RenderTarget::removeAllViewports() {
    clear_map(_sources);
}

int RenderTarget::getWidth() const {
    return _width;
}

int RenderTarget::getHeight() const {
    return _height;
}
