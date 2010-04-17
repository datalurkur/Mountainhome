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

    static VALUE Initialize(VALUE rSelf, VALUE name, VALUE manager, VALUE mat, VALUE rFontName, VALUE text);
    static VALUE AddChild(VALUE rSelf, VALUE child);
    static VALUE EachChild(VALUE rSelf);
    static VALUE CullChild(VALUE rSelf, VALUE child);
    static VALUE CullChildren(VALUE rSelf);

    // Setter Bindings
    static VALUE SetText(VALUE rSelf, VALUE text);
    static VALUE XEquals(VALUE rSelf, VALUE value);
    static VALUE YEquals(VALUE rSelf, VALUE value);

    // Accessor Bindings
    static VALUE GetText(VALUE rSelf);
    static VALUE X(VALUE rSelf);
    static VALUE Y(VALUE rSelf);
    static VALUE W(VALUE rSelf);
    static VALUE H(VALUE rSelf);
    static VALUE XOffset(VALUE rSelf);
    static VALUE YOffset(VALUE rSelf);
    
    // General config bindings
    static VALUE SetDimensions(VALUE rSelf, VALUE x, VALUE y, VALUE w, VALUE h);
    static VALUE SetOffset(VALUE rSelf, VALUE x, VALUE y);
    static VALUE SetBorder(VALUE rSelf, VALUE border);
    static VALUE SetPosition(VALUE rSelf, VALUE x, VALUE y);
    static VALUE MoveRelative(VALUE rSelf, VALUE x, VALUE y);
    static VALUE Resize(VALUE rSelf, VALUE x, VALUE y);
    static VALUE AlwaysOnTop(VALUE rSelf);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIElement declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    // DO NOT CREATE MHUIElement IN C!!!!! ONLY RUBY!!!
    MHUIElement();
    virtual ~MHUIElement();

    void initialize(const std::string &name, MHUIManager *manager, Material *mat, Font *font, const std::string &text);

    void cullChild(MHUIElement *child);
    void cullChildren();

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

