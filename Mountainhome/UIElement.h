#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include "RubyBindings.h"

class Entity;

class UIElement: public Entity, public ManyObjectBinding<UIElement> {
public:
    UIElement();
    ~UIElement();
private:
};

#endif
