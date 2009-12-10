/*
 *  RenderTarget.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderContext.h"
#include "RenderTarget.h"
#include "RenderSource.h"
#include "Viewport.h"

RenderTarget::RenderTarget(int width, int height): _width(width), _height(height) {}
RenderTarget::~RenderTarget() {
    removeAllRenderSources();
}

void RenderTarget::resize(int width, int height) {
    _width = width;
    _height = height;

    std::list<RenderSource*>::iterator itr;
    for (itr = _sources.begin(); itr != _sources.end(); itr++) {
        (*itr)->resize(width, height);
    }
}

void RenderTarget::addRenderSource(RenderSource *source){
    _sources.push_back(source);

    Viewport *view = new Viewport(this);
    view->setRatios(0, 0, 1, 1);
    source->setViewport(view);
    resize(_width, _height);
}

std::list<RenderSource*>::iterator RenderTarget::getIterator(int index) {
    if (index < 0 || index > _sources.size()) {
        Warn("Requested index out of bounds.");
        return _sources.end();
    }

    std::list<RenderSource*>::iterator itr = _sources.begin();
    for (int i = 0; i < index; i++) {
        itr++;
    }

    return itr;
}

RenderSource* RenderTarget::getRenderSource(int index) {
    return *(getIterator(index));
}

void RenderTarget::removeRenderSource(int index){
    std::list<RenderSource*>::iterator itr = getIterator(index);
    _sources.erase(itr);
}

void RenderTarget::removeAllRenderSources(){
    _sources.clear();
}

int RenderTarget::getWidth() const {
    return _width;
}

int RenderTarget::getHeight() const {
    return _height;
}
