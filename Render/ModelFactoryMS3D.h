/*
 *  ModelFactoryMS3D.h
 *  Engine
 *
 *  Created by Brent Wilson on 3/29/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELFACTORYMS3D_H_
#define _MODELFACTORYMS3D_H_
#include <Base/ResourceManager.h>

#include "Model.h"

class ModelFactoryMS3D : public ResourceFactory<Model> {
public:
    ModelFactoryMS3D() {}
    virtual ~ModelFactoryMS3D() {}
    bool canLoad(const std::string &name);
    Model* load(const std::string &name);

};

//class ModelMS3D : public ModelAnimated {
//	void readVerts(FILE* file);
//	void readFaces(FILE* file);
//	void readGroups(FILE* file);
//	void readMats(FILE* file, const char* texname);
//
//	void readAnimation(FILE* file);
//	void readJoints(FILE* file, ModelAnimated::animation &animation);
//	//Sets the initial position of the joints based on each other.
//	void initJoints();
//
//	//Loads a texture into memory.
//	void loadTexture(dword &texture, const char* path, const char* file);
//
//public:
////C'tors
//	ModelMS3D();
//	virtual ~ModelMS3D();
//
////Overloaded Functions
//	virtual bool loadModel(const char* directory, const char* filename);
//	virtual bool loadAnimation(const char *animation);
//
////Tests
//	static void Test();
//};

#endif
