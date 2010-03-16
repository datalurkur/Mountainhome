/*
 *  Viewport.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Viewport.h"
#include "RenderTarget.h"
#include "RenderContext.h"
#include "RenderSource.h"

Viewport::Viewport(RenderSource *source, RenderTarget *target)
: _target(target) {
    addSource(source, 0);
}

Viewport::~Viewport() {}

void Viewport::render(RenderContext* context) {
    int x, y, w, h;
    getPixelDimensions(x, y, w, h);
    context->setViewport(x, y, w, h);

    /* Loop through the sources, rendering them. */
    Info("Rendering viewport's render sources");
	SourceMap::iterator it;
    for (it=_sources.begin() ; it != _sources.end(); it++) {
		Info("Calling render source's render...");
        (*it).second->render(context);
    }
}

void Viewport::resize(int newParentWidth, int newParentHeight) {
    /* Loop through the sources, resizing them. */
    SourceMap::iterator it;
    for (it=_sources.begin() ; it != _sources.end(); it++) {
        (*it).second->resize(_wRatio * newParentWidth, _hRatio * newParentHeight);
    }
}

void Viewport::setRatios(Real x, Real y, Real w, Real h) {
    _xRatio = x;
    _yRatio = y;
    _wRatio = w;
    _hRatio = h;

    // A Change in ratio means a change needs to be sent to the render source.
    resize(_target->getWidth(), _target->getHeight());
}

void Viewport::setPixelDimensions(int x, int y, int w, int h) {
    setRatios(
        static_cast<Real>(x) / static_cast<Real>(_target->getWidth()),
        static_cast<Real>(y) / static_cast<Real>(_target->getHeight()),
        static_cast<Real>(w) / static_cast<Real>(_target->getWidth()),
        static_cast<Real>(h) / static_cast<Real>(_target->getHeight()));
}

void Viewport::getPixelDimensions(int &x, int &y, int &w, int &h) {
    x = _xRatio * _target->getWidth();
    y = _yRatio * _target->getHeight();
    w = _wRatio * _target->getWidth();
    h = _hRatio * _target->getHeight();
}

RenderTarget* Viewport::getTarget() {
    return _target;
}

RenderSource* Viewport::getSource(int index) {
    return _sources.find(index)->second;
}

RenderSource* Viewport::addSource(RenderSource* source, int index) {
    if (_sources.find(index) != _sources.end()) {
        THROW(DuplicateItemError, "Source already exists at z level " << index << "!");
    }
    _sources[index] = source;
    return source;
}