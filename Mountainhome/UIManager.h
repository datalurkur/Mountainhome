#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_
#include "RubyBindings.h"
#include "UIElement.h"
#include <Render/RenderSource.h>

class UIManager : public RenderSource, public ManyObjectBinding<UIManager> {
public:
    UIManager();
    virtual ~UIManager();

    void render(RenderContext *context);

    static void SetupBindings();

protected:
    UIElement *_elements;
};

#endif
