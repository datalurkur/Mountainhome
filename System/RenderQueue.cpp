#include "RenderQueue.h"

RenderQueue::RenderQueue() {
}

RenderQueue::~RenderQueue() {
}

void RenderQueue::addEntity(Entity *toAdd) {
	_entityList.push_back(toAdd);
}

void RenderQueue::render(RenderContext *context) {
	EntityList::iterator entityItr = _entityList.begin();
	
    for (; entityItr != _entityList.end(); entityItr++) {
		(*entityItr)->render(context);
    }
}

void RenderQueue::renderAndClear(RenderContext *context) {
	render(context);
	_entityList.clear();
}

void RenderQueue::renderWithWire(RenderContext *context) {
	context->setFilled();
	render(context);
	context->setWireFrame();
	render(context);
	_entityList.clear();	
}