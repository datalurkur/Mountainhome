#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include <Render/RenderContext.h>
#include "RubyBindings.h"
#include "MHUIManager.h"

class Entity;
class MHUIManager;

class MHUIElement: public Entity, public ManyObjectBinding<MHUIElement> {
public:
    MHUIElement(const std::string name, MHUIManager *manager, const std::string model, const std::string mat);
    virtual ~MHUIElement();

    void render(RenderContext *context);

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE model, VALUE mat);
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE SetParent(VALUE self, VALUE parent);

private:
    float _width, _height;
	MHUIManager *_manager;
    MHUIElement *_parent;
};

#endif
