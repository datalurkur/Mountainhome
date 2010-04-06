#ifndef _MHUIELEMENT_H_
#define _MHUIELEMENT_H_
#include <string>
#include <Render/Entity.h>
#include <Render/RenderContext.h>
#include "RubyBindings.h"
#include "MHUIManager.h"

class MHUIManager;
class Font;

class MHUIElement: public Entity, public RubyBindings<MHUIElement, true> {
//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
public:
    static void SetupBindings();
    static void Mark(MHUIElement *cSelf);

    static VALUE Initialize(VALUE self, VALUE name, VALUE manager, VALUE mat, VALUE rFontName, VALUE text);
    static VALUE CullChild(VALUE self, VALUE child);

    // Setter Bindings
    static VALUE SetText(VALUE self, VALUE text);
    static VALUE XEquals(VALUE self, VALUE value);
    static VALUE YEquals(VALUE self, VALUE value);

    // Accessor Bindings
    static VALUE GetText(VALUE self);
    static VALUE X(VALUE self);
    static VALUE Y(VALUE self);
    static VALUE W(VALUE self);
    static VALUE H(VALUE self);
    
    // General config bindings
    static VALUE SetDimensions(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE SetOffset(VALUE self, VALUE x, VALUE y);
    static VALUE SetBorder(VALUE self, VALUE border);
    static VALUE SetPosition(VALUE self, VALUE x, VALUE y);
    static VALUE MoveRelative(VALUE self, VALUE x, VALUE y);
    static VALUE Resize(VALUE self, VALUE x, VALUE y);
    static VALUE AlwaysOnTop(VALUE self);
    static VALUE AddChild(VALUE self, VALUE child);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    MHUIElement();
    virtual ~MHUIElement();

    void initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font, const std::string &text);

    void cullChild(MHUIElement *child);
    std::list<MHUIElement*> enqueue();
    void render(RenderContext *context);

private:
	MHUIManager *_manager;
    Font *_font;

    std::string _text, _name;
    int _xoffset, _yoffset;
    int _width, _height;
    int _border;
    bool _onTop;



    std::list<MHUIElement*> _children;
};

#endif

