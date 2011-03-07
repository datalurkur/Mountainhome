/*
 *  ModelBone.h
 *  Render
 *
 *  Created by Brent Wilson on on 1/05/10.
 *  Copyright 2010 Brent Wilson.
 *  All rights reserved.
 *
 */

#ifndef _MODELBONE_H_
#define _MODELBONE_H_

#include "Matrix.h"

class ModelBone {
public:
    ModelBone(const std::string &name, unsigned int index, const Matrix &transform, ModelBone *parent, const std::vector<ModelBone *> &children);
    ~ModelBone();

    const std::string & getName();

    const Matrix & getTransform();

    ModelBone * getParent();

    unsigned int getChildCount();

    ModelBone * getChild(int index);

    unsigned int getBoneArrayIndex();

private:
    ModelBone();

private:
    std::string _name;                  //!< The name of this bone.

    unsigned int _index;                //!< The index of this bone in the parent model's bones.

    Matrix _transformation;             //!< The matrix used to transform this bone relative to its parent.

    ModelBone *_parent;                 //!< The parent of this bone.

    std::vector<ModelBone *> _children; //!< Bones that are children of this bone.

};

#endif
