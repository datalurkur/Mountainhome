/*
 *  ModelBone.h
 *  Render
 *
 *  Created by Andrew Jean on on 8/05/10.
 *  Copyright 2010 Mountainhome Project
 *  All rights reserved.
 *
 */

#ifndef _MODELBONE_H_
#define _MODELBONE_H_

class ModelBone {
public:
    ModelBone();
    ~ModelBone();

protected:
    ModelBone *_children;   // Bones that are children of this bone
    ModelBone *_parent;     // The parent of this bone

    std::string _name;      // The name of this bone
    unsigned int _index;    // The index of this bone in the parent model's bones

    // Note: This will probably be a different type eventually
    float *_transform;      // The matrix used to transform this bone relative to its parent
};

#endif
