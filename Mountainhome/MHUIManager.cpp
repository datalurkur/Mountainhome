#include "MHUIManager.h"
#include <Engine/Window.h>
#include <Render/FontManager.h>

MHUIManager::MHUIManager(const std::string looknfeel) {
    // Set up the looknfeel
    // FIXME - load the looknfeel def file and determine the font to be used from there
    _font = FontManager::Get()->getOrLoadResource("example.font");

	MHCore::GetWindow()->addRenderSource(this, 1);
}

MHUIManager::~MHUIManager() {}

void MHUIManager::SetupBindings() {
    Class = rb_define_class("MHUIManager", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIManager::Initialize), 1);
    rb_define_method(Class, "set_root", RUBY_METHOD_FUNC(MHUIManager::SetRoot), 1);
    rb_define_method(Class, "kill_element", RUBY_METHOD_FUNC(MHUIManager::KillElement), 1);
}

VALUE MHUIManager::Initialize(VALUE self, VALUE looknfeel) {
    std::string strLooknfeel = rb_string_value_cstr(&looknfeel);
    RegisterObject(self, new MHUIManager(strLooknfeel));
    return self;
}

VALUE MHUIManager::SetRoot(VALUE self, VALUE element) {
    MHUIManager::GetObject(self)->_rootNode = MHUIElement::GetObject(element);
    return self;
}

VALUE MHUIManager::KillElement(VALUE self, VALUE element) {
    MHUIElement *node = MHUIElement::GetObject(element);
	MHUIElement *root = MHUIManager::GetObject(self)->_rootNode;
    root->cullChild(node);

    return self;
}

void MHUIManager::render(RenderContext* context) {
	context->setDepthTest(false);
	context->setOrtho2D(0, GUI_WIDTH, 0, GUI_HEIGHT);
	context->resetModelviewMatrix();
	context->setFilled();
	context->setLighting(false);
	
	RenderQueue::Get()->addEntity(_rootNode);
    RenderQueue::Get()->renderAndClear(context);

	context->setDepthTest(true);
	context->setLighting(true);
}

void MHUIManager::resize(int width, int height) {
    Info("Resizing UIManager to " << width << " " << height);
    _width = width;
    _height = height;
}

