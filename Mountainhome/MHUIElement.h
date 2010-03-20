#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include <Render/RenderContext.h>
#include "RubyBindings.h"
#include "MHUIManager.h"

class MHUIManager;

class MHUIElement: public Entity, public ManyObjectBinding<MHUIElement> {
public:
    MHUIElement(const std::string name, MHUIManager *manager, const std::string mat, const std::string text);
    virtual ~MHUIElement();

    void render(RenderContext *context);

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat, VALUE text);
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE AddChild(VALUE self, VALUE child);

private:
    float _width, _height;
    std::string _text;

	MHUIManager *_manager;
    std::list<MHUIElement*> _children;
};

#endif

