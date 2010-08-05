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
	Model(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount, unsigned int *indices = NULL, int indexCount = 0);
	virtual ~Model();

    /*! Returns a reference to the model's bounding box. */
    const AABB3& getBoundingBox() const;

	/*! Renders the model using the given context. */
    void render(RenderContext *context);

    /*! Reduces the poly count for the current model based on the model's indices. */
    void doPolyReduction();

    /*! Clears all visual data associated with the model. */
    void clear();

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
    Model();
    void findBounds();
    void generateVBOs();

protected:
    AABB3 _boundingBox;

    Vector2 *_texCoords;
    Vector3 *_verts;
    Vector3 *_norms;
    int _count;

    unsigned int *_indices;
    int _indexCount;

    unsigned int _indexBuffer;
    unsigned int _vertexBuffer;
    unsigned int _normalBuffer;
    unsigned int _texCoordBuffer;

    bool _drawVerts;
    bool _drawNormals;
    bool _drawAABB;

};

#endif /* _CG_MODEL_H */
