/*
 *  Viewport.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Viewport.h"
#include "RenderTarget.h"
#include "RenderContext.h"
#include "RenderSource.h"

Viewport::Viewport(RenderTarget *target)
: _target(target), _clearColor(0, 0, 0, 1) {
    setRatios(0, 0, 1, 1);
}

Viewport::~Viewport() {}

void Viewport::activate(RenderContext* context) {
    getTarget()->enable(); //todo Should this really be here?
    context->setViewport(this); //todo Should this really be here?
    context->clearBuffers(_clearColor); //todo Should this really be here?
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

void Viewport::setBGColor(const Color4 &color) {
    _clearColor = color;
}
