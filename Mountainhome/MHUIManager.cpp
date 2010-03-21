#include "MHUIManager.h"
#include <Engine/Window.h>
#include <Render/FontManager.h>

MHUIManager::MHUIManager(const std::string looknfeel) {
    // Set up the looknfeel
    // FIXME - load the looknfeel def file and determine the font to be used from there
    _font = FontManager::Get()->getOrLoadResource(FontArgsList("firstv2.ttf", 12));

	MHCore::GetWindow()->addRenderSource(this, 1);
}

MHUIManager::~MHUIManager() {}

void MHUIManager::SetupBindings() {
    Class = rb_define_class("MHUIManager", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIManager::Initialize), 1);
}

VALUE MHUIManager::Initialize(VALUE self, VALUE looknfeel) {
    std::string strLooknfeel = rb_string_value_cstr(&looknfeel);
    RegisterObject(self, new MHUIManager(strLooknfeel));
    return self;
}

void MHUIManager::render(RenderContext* context) {
	context->setDepthTest(false);
	context->setOrtho2D(0, 1, 0, 1);
	context->resetModelviewMatrix();
	context->setFilled();
	context->setLighting(false);
	
    ElementMap::iterator elementItr = _elementMap.begin();
    for(; elementItr != _elementMap.end(); elementItr++) {
		RenderQueue::Get()->addEntity(elementItr->second);
    }

    RenderQueue::Get()->renderAndClear(context);

	context->setDepthTest(true);
	context->setLighting(true);
}

void MHUIManager::resize(int width, int height) {}

void MHUIManager::addElement(const std::string name, MHUIElement* element) {
    if(_elementMap.find(name) != _elementMap.end()) {
        THROW(DuplicateItemError, "Element named" << name << "already exists in the UI!");
    }

    _elementMap[name] = element;
}

MHUIElement* MHUIManager::getElement(const std::string name) {
    return _elementMap[name];
}
