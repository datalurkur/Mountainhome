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

class ModelMesh {
public:
    ModelMesh();
    ~ModelMesh();

private:
    AABB3 *_bound;          // The bounding box that contains this mesh
    Material *_materials;   // The materials used in this mesh
    ModelMeshPart *_parts;  // The mesh parts that make up this mesh
    ModelBone *_parent;     // The parent bone for this mesh
    std::string _tag;       // An object that identifies this mesh
};

#endif
