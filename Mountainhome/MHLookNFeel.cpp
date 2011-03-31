/*
 * MHLookNFeel.cpp
 * Mountainhome
 *
 * Created by datalurkur on 2/21/2011
 * Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHLookNFeel.h"
#include <Content/Content.h>
#include <Render/Font.h>

MHLookNFeel::MHLookNFeel()
{}

Renderable *MHLookNFeel::createRectRenderable(int width, int height, int xoffset, int yoffset, const std::string &materialName) {
    xoffset += width/2.0;
    yoffset += height/2.0;
    RenderOperation *renderOp = RenderOperation::CreateRectangleOp(Vector2(width, height), Vector2(xoffset, yoffset));
    Material *mat = Content::GetOrLoad<Material>(materialName);
    return new Renderable(renderOp, mat);
}

Renderable *MHLookNFeel::createTextRenderable(const std::string &fontName, Color4 color, const std::string &text) {
    Font *font = Content::GetOrLoad<Font>(fontName);
    return font->print(color, text.c_str());
}

int MHLookNFeel::getTextWidth(const std::string &fontName, const std::string &text) {
    Font *font = Content::GetOrLoad<Font>(fontName);
    return font->getWidth(text);
}

int MHLookNFeel::getTextHeight(const std::string &fontName) {
    Font *font = Content::GetOrLoad<Font>(fontName);
    return font->getHeight();
}
