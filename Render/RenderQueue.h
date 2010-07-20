/*
 *  RenderQueue.h
 *  Render
 *
 *  Created by Brent Wilson on 1/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RENDERQUEUE_H_
#define _RENDERQUEUE_H_
#include "RenderContext.h"
#include "Singleton.h"

class Renderable;
class RenderQueue : public Singleton<RenderQueue> {
protected:
	RenderQueue();
	virtual ~RenderQueue();
	
	template <class T> friend class Singleton;
	
public:
	void add(Renderable *toAdd);
	void render(RenderContext *context);
	void renderAndClear(RenderContext *context);
	
private:
	typedef std::list<Renderable*> RenderableList;
	
	RenderableList _renderableList;
};

#endif
