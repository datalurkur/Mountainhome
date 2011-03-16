/*
 *  Renderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "Renderable.h"

#if DEBUG
// Ensures that pre/post render calls are not made inappropriately. This ensures
// pushParameters/popParameters will be properly paired.
static bool SafetyCheck_BetweenCalls = false;
#endif

Renderable::Renderable():
    _renderOp(NULL),
    _material(NULL),
    _modelMatrix(Matrix::Identity())
#if DEBUG
    , Parent(NULL)
#endif
{}

Renderable::Renderable(RenderOperation *op, Material *mat):
    _renderOp(op),
    _material(mat),
    _modelMatrix(Matrix::Identity())
#if DEBUG
    , Parent(NULL)
#endif
{}

Renderable::~Renderable() {
    // Do NOT delete the renderop. It may be shared by many things.
}

RenderOperation *Renderable::getRenderOperation() {
    return _renderOp;
}

void Renderable::setMaterial(Material *newMat) {
    _material = newMat;
}

Material * Renderable::getMaterial() {
    return _material;
}

void Renderable::setModelMatrix(const Matrix &mat) {
    _modelMatrix = mat;
}

const Matrix & Renderable::getModelMatrix() {
    return _modelMatrix;
}

void Renderable::preRenderNotice() {
#if DEBUG
    if (SafetyCheck_BetweenCalls) {
        THROW(InternalError, "Nested call to Renderable::preRenderNotice!!!");
    }

    SafetyCheck_BetweenCalls = true;
#endif // DEBUG

    pushParameters(_material->getShader());
}

void Renderable::postRenderNotice() {
#if DEBUG
    if (!SafetyCheck_BetweenCalls) {
        THROW(InternalError, "Out of order call to Renderable::postRenderNotice!!!");
    }

    SafetyCheck_BetweenCalls = false;
#endif // DEBUG

    popParameters();
}

bool Renderable::operator<(const Renderable& rhs) {
    // Sort based on material. Just compare pointers for speed purposes. Comparing names would work, too.
    return _material < rhs._material;
}
