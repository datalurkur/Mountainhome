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

    void render(RenderContext *context);
    void resize(int width, int height);

    void addElement(const std::string name, MHUIElement* element);
    MHUIElement *getElement(const std::string name);
	
	Font *getFont() { return _font; }

private:
    Font *_font;

    typedef std::map<std::string, MHUIElement*> ElementMap;
    ElementMap _elementMap;
};

#endif
