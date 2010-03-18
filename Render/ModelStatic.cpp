/*
 *  ModelFactoryStatic.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 8/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#include "ModelStatic.h"

//#include "Base.h"
//#include "GLExtensions.h"
//#include "TextureManager.h"
//#include "Math3D.h"
//
//ModelStatic::ModelStatic(): Model::Model() {
//	m_nPolygonCount = 0;
//	m_nVertexCount = 0;
//
//	m_vertex = NULL;
//	m_vertexNormal = NULL;
//	m_texCoord = NULL;
//	m_polygon = NULL;
//
//	m_vboVertices = 0;
//	m_vboVertexNormals = 0;
//	m_vboTexCoords = 0;
//	
//	memset(&m_mat, 0, sizeof(material3ds));
//}
//
//ModelStatic::~ModelStatic() {
//	if (VOB_Enabled) {
//		dword nBuffers[3] = { m_vboVertices, m_vboTexCoords, m_vboVertexNormals };
//		glDeleteBuffersARB(3, nBuffers);
//	} else {
//		DeleteArray(m_vertex);
//		DeleteArray(m_vertexNormal);
//		DeleteArray(m_texCoord);
//	}
//	DeleteArray(m_polygon);
//}
//
//void ModelStatic::init()  {
//	calcVertexNormals(); 
//	if (VOB_Enabled) {
//		cg_buildVOB(m_nVertexCount * sizeof(float) * 3, m_vertex, m_vboVertices);
//		cg_buildVOB(m_nVertexCount * sizeof(float) * 3, m_vertexNormal, m_vboVertexNormals);
//		cg_buildVOB(m_nVertexCount * sizeof(float) * 2, m_texCoord, m_vboTexCoords);
//	}
//}
//
//void ModelStatic::calcVertexNormals() {
//	int num_dependants[m_nVertexCount];	
//	memset(num_dependants, 0, m_nVertexCount * sizeof(int));
//	m_vertexNormal = new Vector3[m_nVertexCount];
//	for (int i = 0; i < m_nPolygonCount; i++) {
//		Poly3 &curPolygon = m_polygon[i];
//		
//		Vector3 one = m_vertex[curPolygon[1]] - m_vertex[curPolygon[0]];
//		Vector3 two = m_vertex[curPolygon[2]] - m_vertex[curPolygon[1]];
//		Vector3 polyNormal = CrossProduct(one, two);
//
//		for (int k = 0; k < 3; k++) {
//			num_dependants[curPolygon[k]] += 1; 
//			m_vertexNormal[curPolygon[k]] += polyNormal;
//		}
//	}
//
//	for (int i = 0; i < m_nVertexCount; i++) {
//		if (num_dependants[i] > 0) {
//			m_vertexNormal[i] /= num_dependants[i];
//		}
//	}
//} // Calcnormals
//
//void ModelStatic::applyMaterials() {
//	glMaterialfv(GL_FRONT, GL_AMBIENT,  m_mat.ambient.asPointer());
//	glMaterialfv(GL_FRONT, GL_DIFFUSE,  m_mat.diffuse.asPointer());
//	glMaterialfv(GL_FRONT, GL_SPECULAR, m_mat.specular.asPointer());
//	glMaterialfv(GL_FRONT, GL_EMISSION, m_mat.emissive.asPointer());
//	glMaterialf(GL_FRONT, GL_SHININESS, m_mat.shininess);
//
//	if (m_mat.tex_id) {
//		glBindTexture(GL_TEXTURE_2D, m_mat.tex_id);
//		glEnable(GL_TEXTURE_2D);
//	} else {
//		glDisable(GL_TEXTURE_2D);
//	}
//}
//
//void ModelStatic::render(const Matrix &orientation, dword time) {
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	if (m_mat.tex_id)
//		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	
//	if(VOB_Enabled) {
//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboVertices);
//		glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);
//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboVertexNormals);
//		glNormalPointer(GL_FLOAT, 0, (char *) NULL);
//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboTexCoords);
//		glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
//	} else {
//		glVertexPointer(3, GL_FLOAT, 0, m_vertex);
//		glNormalPointer(GL_FLOAT, 0, m_vertexNormal);
//		glTexCoordPointer(2, GL_FLOAT, 0, m_texCoord);
//	}
//
//	glPushMatrix();
//		applyMaterials();
//		/*glBegin(GL_TRIANGLES);
//		for (int k = 0; k < m_nPolygonCount * 3; k++) {
//			glArrayElement(((word*)m_polygon)[k]);
//		}
//		glEnd();*/
//	glDrawElements(GL_TRIANGLES, m_nPolygonCount * 3, GL_UNSIGNED_INT, (word*)m_polygon);
//	glPopMatrix();
//	glDisable(GL_TEXTURE_2D);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//} //Draw
//
//void ModelStatic::generateBounds(Vector3 &lbf, Vector3 &rtn) {
//	float left, right, top, bottom, near, far;
//	left = right = m_vertex[0][0];
//	top = bottom = m_vertex[0][1];
//	near = far = m_vertex[0][2];
//
//	for (int i = 0; i < m_nVertexCount; i++) {
//		if (m_vertex[i][0] < left) { left = m_vertex[i][0]; }
//		if (m_vertex[i][0] > right) { right = m_vertex[i][0]; }
//		if (m_vertex[i][1] < bottom) { bottom = m_vertex[i][1]; }
//		if (m_vertex[i][1] > top) { top = m_vertex[i][1]; }
//		if (m_vertex[i][2] < far) { far = m_vertex[i][2]; }
//		if (m_vertex[i][2] > near) { near = m_vertex[i][2]; }
//	}
//
//	lbf = Vector3(left, bottom + 10, far); //FIXME hax
//	rtn = Vector3(right, top - 10, near);
//}
//
//bool ModelStatic::loadTexture(const char* filename, word matIndex) {
//	GLuint newTexture = TextureManager::Load(filename);
//	if (!newTexture || matIndex > 1) { return false; }
//	m_mat.tex_id = newTexture;
//	return true;
//}
