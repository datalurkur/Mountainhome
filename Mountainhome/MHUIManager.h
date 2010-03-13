#ifndef _MHUIMANAGER_H_
#define _MHUIMANAGER_H_
#include "RubyBindings.h"
#include "MHUIElement.h"
#include <Render/RenderSource.h>
#include <string>

class MHUIElement;

class MHUIManager : public RenderSource, public ManyObjectBinding<MHUIManager> {
public:
    MHUIManager(const std::string looknfeel);
    virtual ~MHUIManager();

    void render(RenderContext *context);
    void resize(int width, int height);

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE looknfeel);

protected:
    MHUIElement *_elements;
};

#endif
