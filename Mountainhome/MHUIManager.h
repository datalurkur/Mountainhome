#ifndef _MHUIMANAGER_H_
#define _MHUIMANAGER_H_
#include "RubyBindings.h"
#include "MHUIElement.h"
#include "MHCore.h"
#include <Render/RenderSource.h>
#include <Render/RenderQueue.h>
#include <string>

class MHUIElement;
class Font;

class MHUIManager : public RenderSource, public RubyBindings<MHUIManager, true> {
//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIManager ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
public:
    static void SetupBindings();
    static void Mark(MHUIManager *cSelf);

    static VALUE Initialize(VALUE self, VALUE looknfeel, VALUE rcore);
    static VALUE SetRoot(VALUE self, VALUE element);
    static VALUE GetRoot(VALUE self);

    static VALUE GetHeight(VALUE self);
    static VALUE GetWidth(VALUE self);

    static VALUE TextHeight(VALUE self);
    static VALUE TextWidth(VALUE self, VALUE text);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHUIManager declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    MHUIManager();
    virtual ~MHUIManager();

    void initialize(const std::string &looknfeel, MHCore *core);
    void render(RenderContext *context);
    void resize(int width, int height);

    MaterialManager *getMaterialManager();
    FontManager *getFontManager();

	Font *getFont() { return _font; }
    int getWidth() { return _width; }
    int getHeight() { return _height; }

private:
    MaterialManager *_materialManager;
    FontManager *_fontManager;

    MHUIElement *_rootNode;

    Font *_font;
    int _width, _height;

};

#endif
