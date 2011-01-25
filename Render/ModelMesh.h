/*
 *  ModelMesh.h
 *  Render
 *
 *  Created by Brent Wilson on on 1/05/11.
 *  Copyright 2011 Brent Wilson.
 *  All rights reserved.
 *
 */

#ifndef _MODELMESH_H_
#define _MODELMESH_H_
#include <Base/AABB.h>
#include <string>

class Material;
class ModelBone;
class RenderOperation;

class ModelMesh {
public:
    ModelMesh(const std::string & name, RenderOperation * op, Material *mat, ModelBone * root, const AABB3 & bounds);

    ~ModelMesh();

    RenderOperation * getRenderOperation();

    Material * getDefaultMaterial();

    ModelBone * getRootBone();

    const AABB3 & getBoundingBox();

    const std::string & getName();

private:
    ModelMesh();

private:
    std::string _name;          //!< Name of the ModelMesh.

    RenderOperation *_renderOp; //!< The RenderOperation representing the ModelMesh.

    Material *_defaultMaterial; //!< The default material for the ModelMesh.

    ModelBone *_rootBone;       //!< The primary bone for this mesh.

    AABB3 _bounds;              //!< The bounding box of the ModelMesh, in its local space.

};

#endif
