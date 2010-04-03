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

class MHUIManager : public RenderSource, public ManyObjectBinding<MHUIManager> {
public:
    MHUIManager(const std::string looknfeel);
    virtual ~MHUIManager();

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE looknfeel);
    static VALUE SetRoot(VALUE self, VALUE element);
    static VALUE GetWidth(VALUE self);
    static VALUE GetHeight(VALUE self);

    void render(RenderContext *context);
    void resize(int width, int height);

	
	Font *getFont() { return _font; }
    int getWidth() { return _width; }
    int getHeight() { return _height; }

private:
    Font *_font;
    int _width, _height;

    MHUIElement *_rootNode;
};

#endif
