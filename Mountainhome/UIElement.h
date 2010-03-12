#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include "RubyBindings.h"
#include "UIManager.h"

class Entity;
class UIManager;

class UIElement: public Entity, public ManyObjectBinding<UIElement> {
public:
    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE model, VALUE mat);
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);

private:
    float _width, _height;
	UIManager *_manager;
};

#endif
