/*
 *  Renderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Renderable.h"

Renderable::Renderable(): _material(0) {}
Renderable::~Renderable() {}

void Renderable::setMaterial(Material *m) { _material = m; }
Material* Renderable::getMaterial() { return _material; }
void Renderable::postRenderNotice(RenderContext *context) {}
void Renderable::preRenderNotice(RenderContext *context) {}
