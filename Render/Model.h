/*
 *  Model.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson.
 *  All rights reserved.
 *
 */

#ifndef _MODEL_H
#define _MODEL_H
#include <Base/Vector.h>
#include <Base/AABB.h>
#include "ModelMesh.h"
#include "ModelBone.h"

class RenderContext;

class Model {
public:
    static Model * CreateSphere(const std::string &name, Material *mat, int strips, int panels, Real radius, bool wire = false);

    static Model * CreateBox(const std::string &name, Material *mat, const Vector3 &dimensions, bool wire = false);

    static Model * CreateRectangle(const std::string &name, Material *mat, const Vector2 &dimensions, bool wire = false);

public:
	Model(const std::string &name, ModelBone *root, const std::vector<ModelMesh *> &meshes, const std::vector<ModelBone *> &bones);

    Model(const std::string & name, RenderOperation * op, Material *mat, const AABB3 & bounds);

	virtual ~Model();

    /*! Get a reference to the model's bounding box. */
    const AABB3 & getBoundingBox() const;

    /*! Get a reference to the requested modelmesh (given an index). */
    ModelMesh * getMesh(int index);

    /*! Get the number of meshes within the Model. */
    unsigned int getMeshCount();

    /*! Get the bone at the specified index. */
    ModelBone * getBone(int index);

    /*! Get the number of bones in the model. */
    unsigned int getBoneCount();

    /*! Get the central bone of the model. */
    ModelBone * getRootBone();

    /*! Returns the name of the model. */
    const std::string & getName();

protected:
    Model();

private:
    std::string _name;                //!< This model's identifier

    ModelBone *_rootBone;             //!< The root bone for this model.

    std::vector<ModelMesh *> _meshes; //!< The meshes that make up this model.

    std::vector<ModelBone *> _bones;  //!< The bones which describe how each mesh related to its parent mesh.

    AABB3 _bounds;                    //!< Bounding box for the model in its local space.

};

#endif
