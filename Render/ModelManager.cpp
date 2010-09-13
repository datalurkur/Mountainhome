/*
 *  ModelManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/06/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include "ModelManager.h"
#include "Model3DS.h"
#include "ModelMS3D.h"
#include "ModelMD5.h"
#include "ModelFBX.h"

ModelManager::ModelManager(ResourceGroupManager *manager) {
    registerFactory(new Model3DS::Factory());
    registerFactory(new ModelMS3D::Factory());
    registerFactory(new ModelMD5::Factory());
    registerFactory(new ModelFBX::Factory(manager));
}

ModelManager::~ModelManager() {}

//#include "MeshMS3D.h"
//#include "Mesh3DS.h"
//#include "Common.h"
//
//Mesh* ModelManager::Load(const char* dir, const char* filename) {
//	Mesh *model;
//	model = LoadAnimated(dir, filename);
//	if (model) { return model; }
//	model = LoadStatic(dir, filename);
//	if (model) { return model; }
//
//	return NULL;
//}
//
//MeshAnimated* ModelManager::LoadAnimated(const char* dir, const char* filename) {
//	if (!dir || !filename) { 
//		cerr << "ModelManager: Given null directory or filename" << endl;
//		return NULL; 
//	}
//
//	return Get()->loadAnimated(dir, filename);
//}
//
//MeshStatic* ModelManager::LoadStatic(const char* dir, const char* filename) {
//	if (!dir || !filename) { 
//		cerr << "ModelManager: Given null directory or filename" << endl;
//		return NULL; 
//	}
//
//	return Get()->loadStatic(dir, filename);
//}
//
//void ModelManager::unloadModel(const char* name) {
//	DeletePointer(model_map[name]);
//	model_map.erase(name);
//}
//
//void ModelManager::freeModels() {
//	map<const char*, Mesh*>::iterator itr;
//	for (itr = model_map.begin(); itr != model_map.end(); itr++) {
//		unloadModel(itr->first);
//	}
//}
//
//void ModelManager::getModelType(const char* filename, char* type) {
//	char* token;
//	char* temp;
//	token = strchr(filename, '.');
//	if (token == NULL) {
//		cerr << "ModelManager: No file extension found in " << filename << endl;
//		type[0] = '\0';
//		return;	
//	}
//	while ( (temp = strchr(token, '.')) != NULL) {
//		(token = temp)++;
//	}
//	
//	strcpy(type, token);	
//}
//
//MeshAnimated* ModelManager::loadAnimated(const char* dir, const char* file) {
//	MeshAnimated *model;
//	char extension[256];
//	getModelType(file, extension);
//	if (!strcmp(extension, "")){
//		cerr << "ModelManager: Could not get model type from " << string(dir) + file << endl;
//		return NULL;
//	}
//
//	if (!strcmp(extension, "ms3d")) { model = new MeshMS3D(); }
//	//else if (!strcmp(extension, "md5")) { model = new MeshMD5(); }
//	else {
//		cerr << "ModelManager: " << string(dir) + file << " not an animated mesh" << endl;
//		return NULL; 
//	}
//
//	if (!model->loadModel(dir, file)) {
//		cerr << "ModelManager: Error reading model file " << string(dir) + file << endl;
//		return NULL;
//	}
//
//	model_map[file] = model;
//	return model;
//}
//
//MeshStatic* ModelManager::loadStatic(const char* dir, const char* file) {
//	MeshStatic* model;
//	char extension[256];
//	getModelType(file, extension);
//	if (!strcmp(extension, "")){
//		cerr << "ModelManager: Could not get model type" << endl;
//		return NULL;
//	}
//
//	if (!strcmp(extension, "3ds")) { model = new Mesh3DS(); }
//	else { 
//		cerr << "ModelManager: " << string(dir) + file << " not a static mesh" << endl;
//		return NULL; 
//	}
//
//	if (!model->loadModel(dir, file)) {
//		cerr << "ModelManager: Error reading model file" << endl;
//		return NULL;
//	}
//
//	model_map[file] = model;
//	return model;
//}
//
//void ModelManager::Test() {
//	TestLoadStatic();
//	TestLoadAnimated();
//}
//
//void ModelManager::TestLoadAnimated() {
//	MeshMS3D::Test();
//	assert(!LoadAnimated("resources/test/MeshMS3D/", "heart"));
//	assert(!LoadAnimated("resources/test/Mesh3DS/", "ladder.3ds"));
//	assert(!LoadAnimated("resources/test/MeshMS3D/", "null"));
//	assert(!LoadAnimated("resources/test/MeshMS3D/", NULL));
//	assert(LoadAnimated("resources/test/MeshMS3D/", "heart.ms3d"));
//	Kill();
//}
//
//void ModelManager::TestLoadStatic() {
//	Mesh3DS::Test();
//	assert(!LoadStatic("resources/test/Mesh3DS/", "ladder"));
//	assert(!LoadStatic("resources/test/MeshMS3D/", "heart.ms3d"));
//	assert(!LoadStatic("resources/test/Mesh3DS/", "null"));
//	assert(!LoadStatic("resources/test/Mesh3DS/", NULL));
//	assert(LoadStatic("resources/test/Mesh3DS/", "ladder.3ds"));
//	Kill();
//}
