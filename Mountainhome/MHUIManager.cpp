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

#include <Render/RenderContext.h>
#include <Content/Content.h>
#include <Engine/Window.h>

MHUIManager::MHUIManager(): _rootNode(NULL), _font(NULL), _width(0), _height(0) {
    ///\todo load the looknfeel def file and determine the font to be used from there
    _font = Content::GetOrLoad<Font>("example.font");
}

MHUIManager::~MHUIManager() {}

MHUIElement *MHUIManager::getRoot() {
    return _rootNode;
}

void MHUIManager::setRoot(MHUIElement *node) {
    _rootNode = node;
}

void MHUIManager::render(RenderContext* context) {
    RenderableList visibleRenderables;
    _rootNode->addRenderablesToList(visibleRenderables);
    _rootNode->updateDerivedValues();

    context->setDepthTest(false);

    context->render(
        Matrix::Identity(),
        Matrix::Ortho(0, _width, 0, _height),
        visibleRenderables);
}

void MHUIManager::resize(int width, int height) {
    Info("Resizing UIManager to " << width << " " << height);
    _width = width;
    _height = height;

    ///\todo Get rid of this or something :/
    rb_funcall(MHUIManagerBindings::Get()->getValue(this), rb_intern("resize"), 2, INT2NUM(width), INT2NUM(height));
}
