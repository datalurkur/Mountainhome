/*
 *  ModelMesh.h
 *  Render
 *
 *  Created by Andrew Jean on on 8/05/10.
 *  Copyright 2010 Mountainhome Project
 *  All rights reserved.
 *
 */

#ifndef _MODELMESH_H_
#define _MODELMESH_H_

#include <Base/AABB.h>
#include "Material.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

class ModelMesh {
public:
    ModelMesh(ModelMeshPart *parts, unsigned int numParts);
    ~ModelMesh();

    ModelMeshPart *getPart(int index);

    unsigned int getPartCount() { return _numParts; }

protected:
    ModelMesh();

protected:
    AABB3 _bound;           // The bounding box that contains this mesh

    Material *_materials;   // The materials used in this mesh

    ModelMeshPart *_parts;  // The mesh parts that make up this mesh
    unsigned int _numParts;

    ModelBone *_parent;     // The parent bone for this mesh
    std::string _tag;       // An object that identifies this mesh
};

#endif
