#ifndef _RENDERQUEUE_H_
#define _RENDERQUEUE_H_

#include "Entity.h"
#include "RenderContext.h"
#include "Singleton.h"

class RenderQueue : public Singleton<RenderQueue> {
protected:
	RenderQueue();
	~RenderQueue();
	
	template <class T> friend class Singleton;
	
public:
	void addEntity(Entity *toAdd);
	void render(RenderContext *context);
	void renderAndClear(RenderContext *context);
	
private:
	typedef std::list<Entity*> EntityList;
	
	EntityList _entityList;
};

#endif
