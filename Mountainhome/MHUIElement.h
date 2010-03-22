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

    void cullChild(MHUIElement *child);
    void render(RenderContext *context);

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat, VALUE text);
    static VALUE XEquals(VALUE self, VALUE value);
    static VALUE YEquals(VALUE self, VALUE value);
    static VALUE X(VALUE self);
    static VALUE Y(VALUE self);
    static VALUE W(VALUE self);
    static VALUE H(VALUE self);
    
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE SetOffset(VALUE self, VALUE x, VALUE y);
    static VALUE SetPosition(VALUE self, VALUE x, VALUE y);
    static VALUE AddChild(VALUE self, VALUE child);

private:
    int _width, _height, _xoffset, _yoffset;
    std::string _text, _name;

	MHUIManager *_manager;
    std::list<MHUIElement*> _children;
};

#endif

