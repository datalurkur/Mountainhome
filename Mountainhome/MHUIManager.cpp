/*
 *  MHUIManager.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 7/7/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHUIManager.h"
#include "MHUIManagerBindings.h"

#include <Engine/Window.h>
#include <Render/MaterialManager.h>
#include <Render/FontManager.h>

MHUIManager::MHUIManager(): _materialManager(NULL), _fontManager(NULL), _rootNode(NULL),
_font(NULL), _width(0), _height(0) {}

MHUIManager::~MHUIManager() {}

MHUIElement *MHUIManager::getRoot() {
    return _rootNode;
}

void MHUIManager::setRoot(MHUIElement *node) {
    _rootNode = node;
}

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
    context->applyLighting(false);

    std::list<MHUIElement*> topLevel;
    std::list<MHUIElement*>::iterator it;

    // Place the UIElements in the RenderQueue recursively
    topLevel = _rootNode->enqueue();

    // Add any elements which were deferred (for example, those that need to be drawn on top)
    for (it = topLevel.begin(); it != topLevel.end(); it++) {
        RenderQueue::Get()->add(*it);
    }

    // Render the UI
    RenderQueue::Get()->renderAndClear(context);

    context->setDepthTest(true);
    context->applyLighting(true);
}

void MHUIManager::resize(int width, int height) {
    Info("Resizing UIManager to " << width << " " << height);
    _width = width;
    _height = height;

    ///\todo Get rid of this or something :/
    rb_funcall(MHUIManagerBindings::Get()->getValue(this), rb_intern("resize"), 2, INT2NUM(width), INT2NUM(height));
}

MaterialManager* MHUIManager::getMaterialManager() { 
    return _materialManager;
}

FontManager* MHUIManager::getFontManager() {
    return _fontManager;
}
