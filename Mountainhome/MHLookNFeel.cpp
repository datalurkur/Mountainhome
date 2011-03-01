/*
 * MHLookNFeel.cpp
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHLookNFeel.h"
#include "Renderable.h"
#include <Content/Content.h>
#include <Render/Font.h>

MHLookNFeel::MHLookNFeel()
{}

void MHLookNFeel::clearRenderables(MHUIElement *element) {
    Info("Clearing renderables");
    element->deleteAllRenderables();
}

void MHLookNFeel::addRectRenderable(MHUIElement *element, int width, int height, const std::string &materialName) {
    RenderOperation *renderOp = RenderOperation::CreateRectangleOp(Vector2(width, height), false);
    Material *mat = Content::GetOrLoad<Material>(materialName);
    Renderable *renderable = new Renderable(renderOp, mat);

    element->addRenderable(renderable);
}

void MHLookNFeel::addTextRenderable(MHUIElement *element, const std::string &fontName, const std::string &text) {
    // This is just a temporary color, pass this in as part of the arguments
    Color4 color(1.0f, 1.0f, 1.0f, 1.0f);

    Font *font = Content::GetOrLoad<Font>(fontName);
    FontRenderable *renderable = font->print(color, text.c_str());

    element->addRenderable(renderable);
}

int MHLookNFeel::getTextWidth(const std::string &fontName, const std::string &text) {
    Font *font = Content::GetOrLoad<Font>(fontName);
    return font->getWidth(text);
}

int MHLookNFeel::getTextHeight(const std::string &fontName) {
    Font *font = Content::GetOrLoad<Font>(fontName);
    return font->getHeight();
}
