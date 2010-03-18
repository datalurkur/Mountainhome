/*
 *  ModelFactoryMS3D.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#include "ModelFactoryMS3D.h"
#include <Base/FileSystem.h>
#include <Base/Exception.h>
#include <algorithm>

bool ModelFactoryMS3D::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "ms3d";
}

Model *ModelFactoryMS3D::load(const std::string &name) {
    THROW(NotImplementedError, "MS3D model loading has not been implemented, yet!");
}

//#include "TextureManager.h"
//#include "Math3D.h"
//
//// byte-align structures
//#ifdef _MSC_VER
//#	pragma pack( push, packing )
//#	pragma pack( 1 )
//#	define PACK_STRUCT
//#elif defined( __GNUC__ )
//#	define PACK_STRUCT	__attribute__((packed))
//#else
//#	error you must byte-align these structures with the appropriate compiler directives
//#endif
//
//struct ms3d_header_t {
//    char id[10];							// always "MS3D000000"
//    int  version;							// 4
//} PACK_STRUCT;
//
//struct ms3d_vertex_t {
//    byte    flags;							// SELECTED | SELECTED2 | HIDDEN
//    float   vertex[3];						//
//    char    boneId;							// -1 = no bone
//    byte    referenceCount;
//} PACK_STRUCT;
//
//struct ms3d_triangle_t {
//    word  flags;							// SELECTED | SELECTED2 | HIDDEN
//    word  index[3];							//
//    float normal[3][3];						//
//    float s[3];								//
//    float t[3];								//
//    byte  smoothingGroup;					// 1 - 32
//    byte  groupIndex;						//
//} PACK_STRUCT;
//
//struct ms3d_group_t {
//    byte flags;								// SELECTED | HIDDEN
//    char name[32];							//
//    word numtriangles;						//
//    word *triangleIndices;					// the groups group the triangles
//    char materialIndex;						// -1 = no material
//} PACK_STRUCT;
//
//struct ms3d_material_t {
//    char  name[32];							//
//    float ambient[4];						//
//    float diffuse[4];						//
//    float specular[4];						//
//    float emissive[4];						//
//    float shininess;						// 0.0f - 128.0f
//    float transparency;						// 0.0f - 1.0f
//    char  mode;								// 0, 1, 2 is unused now
//    char  texture[128];						// texture.bmp
//    char  alphamap[128];					// alpha.bmp
//} PACK_STRUCT;
//
//struct ms3d_rot_t {
//    float time;								// time in seconds
//    float rotation[3];						// x, y, z angles
//} PACK_STRUCT;
//
//struct ms3d_pos_t {
//    float time;								// time in seconds
//    float position[3];						// local position
//} PACK_STRUCT;
//
//struct ms3d_joint_t {
//    byte  flags;							// SELECTED | DIRTY
//	char  name[32];
//	char  parent[32];
//	float rotation[3];
//	float position[3];
//	
//    word  rotFrameCount;					//
//    word  posFrameCount;					//
//
//    ms3d_rot_t *rotFrame;					// local animation matrices
//    ms3d_pos_t *posFrame;					// local animation matrices
//} PACK_STRUCT;
//
//struct ms3d_comment_t {
//	int index;								// index of group, material or joint
//	int commentLength;						// length of comment (terminating '\0' is not saved), "MC" has comment length of 2 (not 3)
//	char *comment;							// comment
//} PACK_STRUCT;
//
//// Default alignment
//#ifdef _MSC_VER
//#	pragma pack( pop, packing )
//#endif
//
//#undef PACK_STRUCT
//
//int fileSize;
//
//ModelMS3D::ModelMS3D()
//:ModelAnimated::ModelAnimated()
//{}
//
//ModelMS3D::~ModelMS3D() {}
//
//void ModelMS3D::readVerts(FILE* file) {
//	fread(&m_vertexCount, sizeof(word), 1, file);
//	
//	ms3d_vertex_t *vertex_t = new ms3d_vertex_t[m_vertexCount];
//	fread(vertex_t, sizeof(ms3d_vertex_t), m_vertexCount, file);
//
//	m_vertices = new vertex[m_vertexCount];
//	for (dword i = 0; i < m_vertexCount; i++) {
//		m_vertices[i].vertex = Vector3(vertex_t[i].vertex);
//		m_vertices[i].boneId = vertex_t[i].boneId;
//	}
//}
//
//void ModelMS3D::readFaces(FILE* file) {
//	fread(&m_triangleCount, sizeof(word), 1, file);
//	
//	ms3d_triangle_t *triangle_t = new ms3d_triangle_t[m_triangleCount];
//	fread(triangle_t, sizeof(ms3d_triangle_t), m_triangleCount, file);
//	
//	m_triangles = new triangle[m_triangleCount];
//	for (dword i = 0; i < m_triangleCount; i++) {
//		m_triangles[i].smoothingGroup = triangle_t[i].smoothingGroup;
//		m_triangles[i].groupIndex = triangle_t[i].groupIndex;
//
//		for (int k = 0; k < 3; k++) {
//			m_triangles[i].index[k] = triangle_t[i].index[k];
//			m_triangles[i].normal[k] = Vector3(triangle_t[i].normal[k]);
//			m_triangles[i].uv[k][0] = triangle_t[i].s[k];
//			m_triangles[i].uv[k][1] = triangle_t[i].t[k];
//		}
//	}
//}
//
//void ModelMS3D::readGroups(FILE* file) {
//	fread(&m_groupCount, sizeof(word), 1, file);
//	m_groups = new group[m_groupCount];
//	for (dword i = 0; i < m_groupCount; i++) {	
//		fseek(file, sizeof(byte), SEEK_CUR); //flag
//		fseek(file, sizeof(char) * 32, SEEK_CUR); //name
//		fread(&m_groups[i].triCount, sizeof(word), 1, file);
//		m_groups[i].triIndex = new word[m_groups[i].triCount];
//		fread(m_groups[i].triIndex, sizeof(word), m_groups[i].triCount, file);
//		fread(&m_groups[i].matIndex, sizeof(char), 1, file);
//	}
//}
//
//void ModelMS3D::loadTexture(dword &texture, const char* path, const char* file) {
//	texture = 0;
//	char tex[256];
//	strcpy(tex, path);
//	strcat(tex, file);
//
//	char* ptr;
//	while (ptr = strchr(tex, '\\')) {
//		ptr[0] = '/';
//	}
//	//TODO make this cross-platform ready
//	if (strlen(file) > 0) {
//		texture = TextureManager::Load(tex);
//	}
//}
//
//void ModelMS3D::readMats(FILE* file, const char* path) {
//	fread(&m_materialCount, sizeof(word), 1, file);
//	ms3d_material_t *mat_t = new ms3d_material_t[m_materialCount];
//	fread(mat_t, sizeof(ms3d_material_t), m_materialCount, file);
//	
//	m_materials = new material[m_materialCount];
//	for (dword i = 0; i < m_materialCount; i++) {
//		memcpy(&m_materials[i], mat_t[i].ambient, sizeof(float) * 18);
//
//		loadTexture(m_materials[i].texture, path, mat_t[i].texture);
//		loadTexture(m_materials[i].alphamap, path, mat_t[i].alphamap);
//	}
//}
//
//bool ModelMS3D::loadModel(const char* directory, const char* model) {
//	FILE *file = NULL;
//	char filename[256];
//	strcpy(filename, directory);
//	strcat(filename, model);
//
//	if ((file = fopen(filename, "rb")) == NULL) { //Open the file
//		cerr << "Could not open model file: " << filename << endl;
//		return false;	
//	}
//	
//	ms3d_header_t header;
//	fread(&header, sizeof(ms3d_header_t), 1, file);
//	if (strncmp(header.id, "MS3D000000", 10 )) {
//		cerr << "Not an MS3D file: " << filename << endl;
//		return false;
//	}
//	if (header.version < 3 || header.version > 4) {
//		cerr << "Verison " << header.version << " not supported: " << filename << endl;
//		return false;
//	}
//
//	readVerts(file);
//	readFaces(file);
//	readGroups(file);
//	readMats(file, directory);
//	readAnimation(file);
//	setAnimation(0);
//	initJoints();
//
//	fclose(file);
//	return true;
//}
//
//void ModelMS3D::initJoints() {
//	for (dword i = 0; i < m_vertexCount; i++) {
//		if (m_vertices[i].boneId == -1) continue;
//		Vector3 &vert = m_vertices[i].vertex;
//		m_currentAnim->joints[m_vertices[i].boneId].matAbsolute.applyInverse(vert);
//	}
//
//	for (dword i = 0; i < m_triangleCount; i++) {
//		for (int k = 0; k < 3; k++) {
//			vertex &vec = m_vertices[m_triangles[i].index[k]];
//			Vector3 *norm = &m_triangles[i].normal[k];
//			if (vec.boneId == -1) continue;
//			m_currentAnim->joints[vec.boneId].matAbsolute.applyInvRotation(*norm);
//		}
//	}
//}
//
//void ModelMS3D::readJoints(FILE *file, animation &anim) {
//	fread(&anim.jointCount, sizeof(word), 1, file);
//
//	ms3d_joint_t *joint_t = new ms3d_joint_t[anim.jointCount];
//	for (dword i = 0; i < anim.jointCount; i++) {
//		ms3d_joint_t &current = joint_t[i];
//		fread(&current.flags, sizeof(byte), 1, file);
//		fread(current.name, sizeof(char), 32, file);
//		fread(current.parent, sizeof(char), 32, file);
//		fread(current.rotation, sizeof(float), 3, file);
//		fread(current.position, sizeof(float), 3, file);
//		fread(&current.rotFrameCount, sizeof(word), 1, file);
//		fread(&current.posFrameCount, sizeof(word), 1, file);
//
//		current.rotFrame = new ms3d_rot_t[current.rotFrameCount];
//		current.posFrame = new ms3d_pos_t[current.posFrameCount];
//		fread(current.rotFrame, sizeof(ms3d_rot_t), current.rotFrameCount, file);
//		fread(current.posFrame, sizeof(ms3d_pos_t), current.posFrameCount, file);
//	}
//	
//	anim.joints = new joint[anim.jointCount];
//	for (dword i = 0; i < anim.jointCount; i++) {
//		joint &current = anim.joints[i];
//		current.matRelative.fromEuler(joint_t[i].rotation);
//		current.matRelative.setTranslation(joint_t[i].position);
//
//		current.rotFrameCount = joint_t[i].rotFrameCount;
//		current.posFrameCount = joint_t[i].posFrameCount;
//		current.rotFrame = new rotation[current.rotFrameCount];
//		current.posFrame = new translation[current.posFrameCount];
//		for (dword k = 0; k < current.rotFrameCount; k++) {
//			current.rotFrame[k].time = static_cast<int>(joint_t[i].rotFrame[k].time * 1000);
//			current.rotFrame[k].rotation = Vector3(joint_t[i].rotFrame[k].rotation);
//		}
//
//		for (dword k = 0; k < current.posFrameCount; k++) {
//			current.posFrame[k].time = static_cast<int>(joint_t[i].posFrame[k].time * 1000);
//			current.posFrame[k].position = Vector3(joint_t[i].posFrame[k].position);
//		}
//		current.parent = -1;
//
//		if (strlen(joint_t[i].parent) > 0) {
//			for (dword k = 0; k < anim.jointCount; k++) {
//				if (!strcmp(joint_t[i].parent, joint_t[k].name)) {
//					current.parent = k;
//					break;
//				}
//			}
//		}
//	}
//
//	for (dword i = 0; i < anim.jointCount; i++) {
//		joint &current = anim.joints[i];
//		if (current.parent != -1) {
//			current.matAbsolute.set(anim.joints[current.parent].matAbsolute);
//			current.matAbsolute.postMultiply(current.matRelative);
//		} else {
//			current.matAbsolute.set(current.matRelative);
//		}
//	}
//}
//
//void ModelMS3D::readAnimation(FILE *file) {
//	animation anim;
//	float fps, currTime;
//	int frameCount;
//
//	fread(&fps, sizeof(float), 1, file);
//	fread(&currTime, sizeof(float), 1, file);
//	fread(&frameCount, sizeof(int), 1, file);
//	anim.animationTime = 0;
//	if (fps != 0) {
//		anim.animationTime = static_cast<int>(frameCount * (1000.0f / fps));
//	}
//	readJoints(file, anim);
//	restart(anim);
//	m_animations.push_back(anim);
//}
//
//bool ModelMS3D::loadAnimation(const char* animation) {
//	FILE *file = NULL;
//
//	if ((file = fopen(animation, "rb")) == NULL) { //Open the file
//		cerr << "Could not open model file: " << animation << endl;
//		return false;	
//	}
//	
//	ms3d_header_t header;
//	fread(&header, sizeof(ms3d_header_t), 1, file);
//	if (strncmp(header.id, "MS3D000000", 10 )) {
//		cerr << "Not an MS3D file: " << animation << endl;
//		return false;
//	}
//
//	if (header.version < 3 || header.version > 4) {
//		cerr << "Verison " << header.version << " not supported: " << animation << endl;
//		return false;
//	}
//
//	word offset = 0;
//	//skip vertices
//	fread(&offset, sizeof(word), 1, file);
//	if (offset != m_vertexCount) { cerr << animation << " does not match model" << endl; }
//	fseek(file, sizeof(ms3d_vertex_t) * offset, SEEK_CUR);
//
//	//skip faces
//	fread(&offset, sizeof(word), 1, file);
//	if (offset != m_triangleCount) { cerr << animation << " does not match model" << endl; }
//	fseek(file, sizeof(ms3d_triangle_t) * offset, SEEK_CUR);
//
//	//skip groups
//	fread(&offset, sizeof(word), 1, file);
//	if (offset != m_groupCount) { cerr << animation << " does not match model" << endl; }
//	for (int i = offset; i > 0; i--) {
//		fseek(file, sizeof(byte), SEEK_CUR); //flag
//		fseek(file, sizeof(char) * 32, SEEK_CUR); //name
//		fread(&offset, sizeof(word), 1, file);
//		fseek(file, sizeof(word) * offset, SEEK_CUR);
//		fseek(file, sizeof(char), SEEK_CUR);
//	}
//
//	//skip materials
//	fread(&offset, sizeof(word), 1, file);
//	if (offset != m_materialCount) { cout << animation << " does not match model" << endl; }
//	fseek(file, sizeof(ms3d_material_t) * offset, SEEK_CUR);
//	readAnimation(file);
//	fclose(file);
//	return true;	
//}
//
//void ModelMS3D::Test() {
//	ModelMS3D *m = new ModelMS3D();
//	assert(m->loadModel("resources/test/ms3d/", "hardcore.ms3d"));
//	assert(m->m_vertexCount ==  1620);
//	assert(m->m_animations[0].jointCount == 47);
//	assert(m->m_animations.size() == 1);
//	assert(m->loadAnimation("resources/test/ms3d/hardcore.ms3d"));
//	assert(m->m_vertexCount == 1620);
//	assert(m->m_animations[0].jointCount == 47);
//	assert(m->m_animations.size() == 2);
//	Vector3 a, b, diff;
//	m->generateBounds(a, b);
//	diff = Absolute(a - b);
//	assert(eq(diff[0], 119.188));
//	assert(eq(diff[1], 257.414));
//	assert(eq(diff[2], 55.8513));
//	DeletePointer(m);
//}
