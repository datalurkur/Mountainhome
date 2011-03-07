/*
 *  ModelMesh.h
 *  Render
 *
 *  Created by Brent Wilson on on 1/05/11.
 *  Copyright 2011 Brent Wilson.
 *  All rights reserved.
 *
 */

#include "ModelMesh.h"
#include "ModelBone.h"
#include "Material.h"
#include "RenderOperation.h"

ModelMesh::ModelMesh(
    const std::string & name,
    RenderOperation * op,
    Material *mat,
    ModelBone * root,
    const AABB3 & bounds
):
    _name(name),
    _renderOp(op),
    _defaultMaterial(mat),
    _rootBone(root),
    _bounds(bounds)
{}

ModelMesh::ModelMesh():
    _name("NO NAME"),
    _renderOp(NULL),
    _defaultMaterial(NULL),
    _rootBone(NULL),
    _bounds()
{}

ModelMesh::~ModelMesh() {
    delete _renderOp;
}

const std::string & ModelMesh::getName() {
    return _name;
}

RenderOperation * ModelMesh::getRenderOperation() {
    return _renderOp;
}

Material * ModelMesh::getDefaultMaterial() {
    return _defaultMaterial;
}

ModelBone * ModelMesh::getRootBone() {
    return _rootBone;
}

const AABB3 & ModelMesh::getBoundingBox() {
    return _bounds;
}
