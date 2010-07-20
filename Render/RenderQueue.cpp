/*
 *  RenderQueue.cpp
 *  Render
 *
 *  Created by Brent Wilson on 1/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RenderQueue.h"
#include "Renderable.h"

RenderQueue::RenderQueue() {}

RenderQueue::~RenderQueue() {}

void RenderQueue::add(Renderable *toAdd) {
	_renderableList.push_back(toAdd);
}

void RenderQueue::render(RenderContext *context) {
	RenderableList::iterator itr = _renderableList.begin();

    //Info("Rendering " << _entityList.size() << " entities.");
    for (; itr != _renderableList.end(); itr++) {
        (*itr)->preRenderNotice(context);
		(*itr)->render(context);
        (*itr)->postRenderNotice(context);
    }
}

void RenderQueue::renderAndClear(RenderContext *context) {
	render(context);
	_renderableList.clear();
}
