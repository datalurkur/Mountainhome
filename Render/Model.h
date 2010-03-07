/*
 *  Model.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODEL_H
#define _MODEL_H
#include <Base/Vector.h>
#include <Base/AABB.h>

class RenderContext;

class Model {
public:
	Model();
	virtual ~Model();

//Functions
    const AABB3& getBoundingBox() const;

	//Renders the model to screen.
	virtual void render(RenderContext *context) = 0;

//	//Loads the given model using the given directory as the active one.
//	virtual bool loadModel(const char* directory, const char* filename) = 0;
//
//	//Loads a texture based on the given filename and puts it in the material
//	//based on the given material index.
//	virtual bool loadTexture(const char* filename, word materialIndex) = 0;
//
//	//This gets the lowest and hightest point along all axis in the model.
//	virtual void generateBounds(Vector3 &lbf, Vector3 &rtn) = 0;

protected:
    AABB3 _boundingBox;

};

#endif /* _CG_MODEL_H */
