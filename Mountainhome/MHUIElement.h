#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include "RubyBindings.h"
#include "MHUIManager.h"

class Entity;
class MHUIManager;

class MHUIElement: public Entity, public ManyObjectBinding<MHUIElement> {
public:
    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE model, VALUE mat);
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);

private:
    float _width, _height;
	MHUIManager *_manager;
};

#endif
