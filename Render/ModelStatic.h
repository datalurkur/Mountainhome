/*
 *  ModelFactoryStatic.h
 *  Engine
 *
 *  Created by Brent Wilson on 8/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELSTATIC_H
#define _MODELSTATIC_H

#include "Base.h"
#include "Vector.h"
#include "Matrix.h"
#include "Model.h"


//class ModelStatic : public Model {
//public:
//	ModelStatic();
//	virtual ~ModelStatic();
//	virtual void render(const Matrix &orientation, dword elapsed);
//
//	virtual void generateBounds(Vector3 &lbf, Vector3 &rtn);
//	virtual bool loadTexture(const char* filename, word materialIndex = 0);
//	virtual bool loadModel(const char* directory, const char* filename) = 0;
//
//protected:
//	struct material3ds {
//		Vector4f ambient;
//		Vector4f diffuse;
//		Vector4f specular;
//		Vector4f emissive;
//		float shininess;
//		GLuint tex_id;
//	};
//
//	char name[20];
//	
//	word m_nPolygonCount;
//	word m_nVertexCount;
//
//	Vector3	*m_vertex;
//	Vector3	*m_vertexNormal;
//	Vector2f	*m_texCoord;
//	float		*m_celShader;
//	Poly3		*m_polygon;
//
//	dword m_vboVertices;
//	dword m_vboVertexNormals;
//	dword m_vboCelShader;
//	dword m_vboTexCoords;
//
//	material3ds m_mat;
//	
//	void init();
//	void calcVertexNormals();
//	void applyMaterials();
//
//};

#endif
