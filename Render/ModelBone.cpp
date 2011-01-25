/*
 *  ModelBone.h
 *  Render
 *
 *  Created by Brent Wilson on on 1/05/10.
 *  Copyright 2010 Brent Wilson.
 *  All rights reserved.
 *
 */

#include "ModelBone.h"

ModelBone::ModelBone(
    const std::string &name,
    unsigned int index,
    const Matrix &transform,
    ModelBone *parent,
    const std::vector<ModelBone *> &children
):
    _name(name),
    _index(index),
    _transformation(transform),
    _parent(parent),
    _children(children)
{}

ModelBone::ModelBone():
    _name("NO NAME"),
    _index(0),
    _parent(NULL)
{}

ModelBone::~ModelBone() {
    // Don't destroy the children or parent. Model takes care of all bones.
}

const std::string & ModelBone::getName() {
    return _name;
}

const Matrix & ModelBone::getTransform() {
    return _transformation;
}

ModelBone * ModelBone::getParent() {
    return _parent;
}

unsigned int ModelBone::getChildCount() {
    return _children.size();
}

ModelBone * ModelBone::getChild(int index) {
    return _children[index];
}

unsigned int ModelBone::getBoneArrayIndex() {
    return _index;
}
