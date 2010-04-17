#include "MHUIManager.h"
#include <Engine/Window.h>
#include <Render/MaterialManager.h>
#include <Render/FontManager.h>

void MHUIManager::SetupBindings() {
    Class = rb_define_class("MHUIManager", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHUIManager::Initialize), 2);
    rb_define_method(Class, "set_root", RUBY_METHOD_FUNC(MHUIManager::SetRoot), 1);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHUIManager::GetHeight), 0);
    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHUIManager::GetWidth), 0);
    rb_define_method(Class, "text_height", RUBY_METHOD_FUNC(MHUIManager::TextHeight), 0);
    rb_define_method(Class, "text_width", RUBY_METHOD_FUNC(MHUIManager::TextWidth), 1);
    rb_define_alloc_func(Class, MHUIManager::Alloc);
}

VALUE MHUIManager::Initialize(VALUE rSelf, VALUE looknfeel, VALUE rCore) {
    std::string strLooknfeel = rb_string_value_cstr(&looknfeel);
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    AssignCObjFromValue(MHCore, cCore, rCore);
    cSelf->initialize(strLooknfeel, cCore);
    return rSelf;
}

VALUE MHUIManager::SetRoot(VALUE rSelf, VALUE rElement) {
    AssignCObjFromValue(MHUIElement, cElement, rElement);
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    cSelf->_rootNode = cElement;
    return rSelf;
}

VALUE MHUIManager::GetWidth(VALUE rSelf) {
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHUIManager::GetHeight(VALUE rSelf) {
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHUIManager::TextWidth(VALUE rSelf, VALUE rText) {
    std::string cText = rb_string_value_cstr(&rText);
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    return INT2NUM(cSelf->_font->getWidth(cText));
}

VALUE MHUIManager::TextHeight(VALUE rSelf) {
    AssignCObjFromValue(MHUIManager, cSelf, rSelf);
    return INT2NUM(cSelf->_font->getHeight());
}

MHUIManager::MHUIManager(): _materialManager(NULL), _fontManager(NULL), _rootNode(NULL),
_font(NULL), _width(0), _height(0) {}

MHUIManager::~MHUIManager() {}

void MHUIManager::initialize(const std::string &looknfeel, MHCore *core) {
    _materialManager = core->getMaterialManager();
    _fontManager = core->getFontManager();

    ///\todo load the looknfeel def file and determine the font to be used from there
    _font = _fontManager->getOrLoadResource("example.font");
}

void MHUIManager::render(RenderContext* context) {
    context->setDepthTest(false);
    context->setOrtho2D(0, _width, 0, _height);
    context->resetModelviewMatrix();
    context->setFilled();
    context->setLighting(false);

    std::list<MHUIElement*> topLevel;
    std::list<MHUIElement*>::iterator it;

    // Place the UIElements in the RenderQueue recursively
    topLevel = _rootNode->enqueue();

    // Add any elements which were deferred (for example, those that need to be drawn on top)
    for(it=topLevel.begin(); it!=topLevel.end(); it++) {
        RenderQueue::Get()->addEntity(*it);
    }

    // Render the UI
    RenderQueue::Get()->renderAndClear(context);

    context->setDepthTest(true);
    context->setLighting(true);
}

void MHUIManager::resize(int width, int height) {
    Info("Resizing UIManager to " << width << " " << height);
    _width = width;
    _height = height;

    ///\todo Get rid of this or something :/
    rb_funcall(MHUIManager::GetValue(this), rb_intern("resize"), 2, INT2NUM(width), INT2NUM(height));
}

MaterialManager* MHUIManager::getMaterialManager() { 
    return _materialManager;
}

FontManager* MHUIManager::getFontManager() {
    return _fontManager;
}
