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

MHLookNFeel::MHLookNFeel()
{}

void MHLookNFeel::clearRenderables(MHUIElement *element) {
    Info("Clearing renderables");
    element->deleteAllRenderables();
}

void MHLookNFeel::addRectRenderable(MHUIElement *element, int width, int height, const std::string &materialName) {
    Info("Adding rect of size " << width << " and " << height << " to element.");

    RenderOperation *renderOp = RenderOperation::CreateRectangleOp(Vector2(width, height), false);
    Material *mat = Content::GetOrLoad<Material>(materialName);
    Renderable *renderable = new Renderable(renderOp, mat);

    element->addRenderable(renderable);
}

void MHLookNFeel::addTextRenderable(MHUIElement *element, const std::string &text) {
    Info("Adding text " << text << " to element.");
}
