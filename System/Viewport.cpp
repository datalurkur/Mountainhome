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
: _source(source), _target(target) {}

Viewport::~Viewport() {}

void Viewport::render(RenderContext* context) {
    int x, y, w, h;
    getPixelDimensions(x, y, w, h);
    context->setViewport(x, y, w, h);
    _source->render(context);
}

void Viewport::setRatios(Real x, Real y, Real w, Real h) {
    _xRatio = x;
    _yRatio = y;
    _wRatio = w;
    _hRatio = h;
}

void Viewport::setPixelDimensions(int x, int y, int w, int h) {
    _xRatio = static_cast<Real>(x) / static_cast<Real>(_target->getWidth());
    _wRatio = static_cast<Real>(w) / static_cast<Real>(_target->getWidth());

    _yRatio = static_cast<Real>(y) / static_cast<Real>(_target->getHeight());
    _hRatio = static_cast<Real>(h) / static_cast<Real>(_target->getHeight());
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

RenderSource* Viewport::getSource() {
    return _source;
}
