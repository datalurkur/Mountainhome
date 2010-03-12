#ifndef _MHUIMANAGER_H_
#define _MHUIMANAGER_H_
#include "RubyBindings.h"
#include "MHUIElement.h"
#include <Render/RenderSource.h>

class MHUIElement;

class MHUIManager : public RenderSource, public ManyObjectBinding<MHUIManager> {
public:
    MHUIManager();
    virtual ~MHUIManager();

    void render(RenderContext *context);

    static void SetupBindings();
    static VALUE Initialize(VALUE self);

protected:
    MHUIElement *_elements;
};

#endif
