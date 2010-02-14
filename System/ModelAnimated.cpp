/*
 *  ModelAnimated.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/7/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include "TextureManager.h"
#include "ModelAnimated.h"

//ModelAnimated::ModelAnimated(): Model(), m_triangleCount(0), m_vertexCount(0),
//m_materialCount(0), m_groupCount(0), m_vertices(NULL), m_materials(NULL),
//m_triangles(NULL), m_groups(NULL), m_currentAnim(NULL), m_animationTime(0) {}
//
//ModelAnimated::~ModelAnimated() {
//	DeleteArray(m_vertices);
//	DeleteArray(m_materials);
//	DeleteArray(m_triangles);
//	DeleteArray(m_groups);
//	for (uint i = 0; i < m_animations.size(); i++) {
//		DeleteArray(m_animations[i].joints);
//	}
//}
//
//bool ModelAnimated::loadTexture(const char* filename, word matIndex) {
//	GLuint newTexture = TextureManager::Load(filename);
//	if (!newTexture || matIndex > m_materialCount) { return false; }
//	m_materials[matIndex].texture = newTexture;
//	return true;
//}
//
//void ModelAnimated::restart(animation &anim) {
//	for (int i = 0; i < anim.jointCount; i++) {
//		anim.joints[i].curRotFrame = 0;
//		anim.joints[i].curPosFrame = 0;
//		anim.joints[i].matFinal.set(anim.joints[i].matAbsolute);
//	}
//}
//
//dword ModelAnimated::setAnimation(dword index) {
//	if (index >= m_animations.size()) { return 0; }
//	if (m_currentAnim) { restart(*m_currentAnim); }
//	m_currentAnim = &m_animations[index];
//	return m_currentAnim->animationTime;
//}
//
//Vector3 ModelAnimated::rotateJoint(joint &joint, dword elapsed) {
//	Vector3 retVal;
//	while (joint.rotFrame[joint.curRotFrame].time <= elapsed) {
//		joint.curRotFrame++;
//		if (joint.curRotFrame >= joint.rotFrameCount) {
//			joint.curRotFrame = 0;
//			break;
//		}
//	}
//
//	int last = joint.rotFrameCount - 1;
//	if (joint.curRotFrame != 0) { last = joint.curRotFrame - 1; }
//	rotation *lFrame = &joint.rotFrame[last];
//	rotation *frame = &joint.rotFrame[joint.curRotFrame];
//
//	float frameDelta = frame->time - lFrame->time;
//	if (frameDelta < 0) {
//		frameDelta = m_currentAnim->animationTime - lFrame->time;
//	}
//
//	float percent = 1;
//	if (frameDelta > 0) {
//		percent = static_cast<float>(elapsed - lFrame->time) / frameDelta;
//	}
//
//	retVal = Interpolate(lFrame->rotation, frame->rotation, percent);
//	return retVal;
//}
//
//Vector3 ModelAnimated::translateJoint(joint &joint, dword elapsed) {
//	Vector3 retVal;
//	while (joint.posFrame[joint.curPosFrame].time <= elapsed) {
//		joint.curPosFrame++;
//		if (joint.curPosFrame >= joint.posFrameCount) {
//			joint.curPosFrame = 0;
//			break;
//		}
//	}
//
//	int last = joint.posFrameCount - 1;
//	if (joint.curPosFrame != 0) { last = joint.curPosFrame - 1; }
//	translation *lFrame = &joint.posFrame[last];
//	translation *frame = &joint.posFrame[joint.curPosFrame];
//
//
//	float frameDelta = frame->time - lFrame->time;
//	if (frameDelta < 0) {
//		frameDelta = m_currentAnim->animationTime - lFrame->time;
//	}
//
//	float percent = 1;
//	if (frameDelta != 0) {
//		percent = static_cast<float>(elapsed - lFrame->time) / frameDelta;
//	}
//
//	retVal = Interpolate(lFrame->position, frame->position, percent);
//	return retVal;
//}
//
//void ModelAnimated::animate(dword elapsed) {
//	m_animationTime += elapsed;
//	if (m_animationTime >= static_cast<dword>(m_currentAnim->animationTime)) {
//		m_animationTime = 0;
//	}
//
//	//animate
//	for (int i = 0; i < m_currentAnim->jointCount; i++) {
//		joint &joint = m_currentAnim->joints[i];
//		Matrix animatedRelative(joint.matRelative);
//		Matrix midKeyframe(rotateJoint(joint, m_animationTime));
//		midKeyframe.setTranslation(translateJoint(joint, m_animationTime));
//
//		animatedRelative.postMultiply(midKeyframe);
//		if (joint.parent != -1) {
//			joint.matFinal.set(m_currentAnim->joints[joint.parent].matFinal);
//			joint.matFinal.postMultiply(animatedRelative);
//		} else {
//			joint.matFinal.set(animatedRelative);
//		}
//	}
//}
//
//void ModelAnimated::renderJoints() {
//	glColor3f(1, 1, 0);
//	glLineWidth(5);
//	glBegin(GL_LINES);
//		for (int i = 0; i < m_currentAnim->jointCount; i++) {
//			if (m_currentAnim->joints[i].parent < 0)
//				continue;
//			const float *f1 = m_currentAnim->joints[i].matFinal.getMatrix();
//			const float *f2 = m_currentAnim->joints[m_currentAnim->joints[i].parent].matFinal.getMatrix();
//
//			glVertex3f(f1[12], f1[13], f1[14]);
//			glVertex3f(f2[12], f2[13], f2[14]);
//		}
//	glEnd();
//	glColor3f(1,1,1);
//}
//
//void ModelAnimated::setMaterial(char index) {
//	if (index < 0) return;
//
//	material &mat = m_materials[index];
//
//	glMaterialfv(GL_FRONT, GL_AMBIENT,  mat.ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat.diffuse);
//	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat.specular);
//	glMaterialfv(GL_FRONT, GL_EMISSION, mat.emissive);
//	glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);	
//		
//	if (mat.texture > 0) {
//		glActiveTextureARB(GL_TEXTURE1_ARB);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, mat.texture);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
//		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
//	} else {
//		glDisable(GL_TEXTURE_2D);
//	}
//}
//
//void ModelAnimated::renderNormals(const Matrix &orientation) {
//	glColor3f(1,0,0);
//	glLineWidth(2);
//	for (dword i = 0; i < m_groupCount; i++) {
//		glPushMatrix();
//		glBegin(GL_LINES);
//		for (int k = 0; k < m_groups[i].triCount; k++) {
//			triangle &current = m_triangles[m_groups[i].triIndex[k]];
//			for (int j = 0; j < 3; j++) {
//				int joint = m_vertices[current.index[j]].boneId;
//				Vector3 vertex = m_vertices[current.index[j]].vertex;
//				Vector3 normal = current.normal[j];
//				orientation.applyRotation(normal);
//				if (joint != -1) {
//					m_currentAnim->joints[joint].matFinal.applyRotation(normal);
//					m_currentAnim->joints[joint].matFinal.apply(vertex);
//				}
//				glVertex3fv(vertex.asPointer());
//				vertex += normal * 20;
//				glVertex3fv(vertex.asPointer());
//			}
//		}
//		glEnd();
//		glPopMatrix();
//	}
//	glColor3f(1,1,1);
//}
//
//void ModelAnimated::renderModel(const Matrix &orientation) {
//	if (MessageCenter::CheckFlag("cel-shading")) {
//		glActiveTextureARB(GL_TEXTURE0_ARB);
//		glEnable(GL_TEXTURE_1D);
//		glBindTexture(GL_TEXTURE_1D, MessageCenter::Shader());
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
//		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
//	}
//
//	glColor3f(1,1,1);
//	for (dword i = 0; i < m_groupCount; i++) {
//		setMaterial(m_groups[i].matIndex);
//		glBegin(GL_TRIANGLES);
//		for (int k = 0; k < m_groups[i].triCount; k++) {
//			triangle &current = m_triangles[m_groups[i].triIndex[k]];
//			for (int j = 0; j < 3; j++) {
//				int joint = m_vertices[current.index[j]].boneId;
//				Vector3 vertex = m_vertices[current.index[j]].vertex;
//				Vector3 normal = current.normal[j];
//				if (joint != -1) {
//					m_currentAnim->joints[joint].matFinal.applyRotation(normal);
//					m_currentAnim->joints[joint].matFinal.apply(vertex);
//				}
//
//				if (MessageCenter::CheckFlag("cel-shading")) {
//					orientation.applyRotation(normal);
//					glMultiTexCoord1fARB(GL_TEXTURE0_ARB, MessageCenter::CalcLighting(vertex, normal));
//				}
//
//				glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, current.uv[j].asPointer());
//
//				glNormal3fv(normal.asPointer());
//				glVertex3fv(vertex.asPointer());
//			}
//		}
//		glEnd();
//		glDisable(GL_TEXTURE_2D);
//	}
//	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glDisable(GL_TEXTURE_1D);
//}
//
//void ModelAnimated::outline() {
//	beginOutline();
//
//	for (dword i = 0; i < m_groupCount; i++) {
//		glPushMatrix();
//		glBegin(GL_TRIANGLES);
//		for (int k = 0; k < m_groups[i].triCount; k++) {
//			triangle &current = m_triangles[m_groups[i].triIndex[k]];
//			for (int j = 0; j < 3; j++) {
//				int joint = m_vertices[current.index[j]].boneId;
//				Vector3 vertex = m_vertices[current.index[j]].vertex;
//				if (joint != -1) {
//					m_currentAnim->joints[joint].matFinal.apply(vertex);
//				}
//				glVertex3fv(vertex.asPointer());
//			}
//		}
//		glEnd();
//		glPopMatrix();
//	}
//
//	endOutline();
//}
//
//void ModelAnimated::render(const Matrix &orientation, dword elapsed) {
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	animate(elapsed);
//	if (MessageCenter::CheckFlag("lighting")) { glEnable(GL_LIGHTING); }
////	renderJoints();
//	renderModel(orientation);
//	if (MessageCenter::CheckFlag("normals")) { renderNormals(orientation); }
//	if (MessageCenter::CheckFlag("outline")) { outline(); }
//	glDisable(GL_CULL_FACE);
//} //render
//
//void ModelAnimated::generateBounds(Vector3 &lbf, Vector3 &rtn) {
//	float left, right, top, bottom, near, far;
//	left = right = m_vertices[0].vertex[0];
//	top = bottom = m_vertices[0].vertex[1];
//	near = far = m_vertices[0].vertex[2];
//
//	for (dword i = 0; i < m_groupCount; i++) {
//		for (int k = 0; k < m_groups[i].triCount; k++) {
//			triangle &current = m_triangles[m_groups[i].triIndex[k]];
//			for (int j = 0; j < 3; j++) {
//				int joint = m_vertices[current.index[j]].boneId;
//				Vector3 vertex = m_vertices[current.index[j]].vertex;
//				if (joint != -1) {
//					m_currentAnim->joints[joint].matFinal.apply(vertex);
//				}
//
//				if (vertex[0] < left) { left = vertex[0]; }
//				if (vertex[0] > right) { right = vertex[0]; }
//				if (vertex[1] < bottom) { bottom = vertex[1]; }
//				if (vertex[1] > top) { top = vertex[1]; }
//				if (vertex[2] < far) { far = vertex[2]; }
//				if (vertex[2] > near) { near = vertex[2]; }
//			}
//		}
//	}
//
//	lbf = Vector3(left, bottom + 10, far); //FIXME hax
//	rtn = Vector3(right, top - 10, near);
//}
