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
#include "ModelMesh.h"
#include "ModelBone.h"

class RenderContext;

class Model {
public:
	Model(Vector3 *verts, Vector3 *norms, Vector2 *texCoords, int vertexCount, unsigned int *indices = NULL, int indexCount = 0);
	virtual ~Model();

    /*! Returns a reference to the model's bounding box. */
    const AABB3& getBoundingBox() const;

    /*! Returns a reference to the requested modelmesh (given an index). */
    ModelMesh *getMesh(int index);

    /*! Returns the number of meshes within the Model. */
    unsigned int getMeshCount() { return _numMeshes; }

    /*! Clears all visual data associated with the model. */
    void clear();

    int getIndexCount() { return _indexCount; }

    unsigned int getIndexBuffer() { return _indexBuffer; }
    unsigned int getVertexBuffer() { return _vertexBuffer; }
    unsigned int getNormalBuffer() { return _normalBuffer; }
    unsigned int getTexCoordBuffer() { return _texCoordBuffer; }

    unsigned int *getIndices() { return _indices; }

    Vector3 *getVertices() { return _verts; }
    Vector3 *getNormals() { return _norms; }

    bool shouldDrawNormals() { return _drawNormals; }
    bool wireframeMode() { return _wireframe; }

    void setWireframe(bool state) { _wireframe = state; }

    void setDefaultMaterial(Material *mat);
    Material *getDefaultMaterial() { return _defaultMaterial; }

    ModelBone *getRootBone() { return _root; }
    void setRootBone(ModelBone *root) { _root = root; }

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
    void generateNormals();

protected:
    unsigned int _numMeshes;

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
    bool _wireframe;

    ModelBone *_bones;  // The bones which describe how each mesh related to its parent mesh

    ModelMesh *_meshes; // The meshes that make up this model

    ModelBone *_root;   // The root bone for this model
    std::string _tag;   // This model's identifier

    Material *_defaultMaterial;
};

#endif /* _CG_MODEL_H */
