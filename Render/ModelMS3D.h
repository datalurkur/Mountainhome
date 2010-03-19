/*
 *  ModelMS3D.h
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

class ModelMS3D : public Model {
public:
public:
    static void Setup();
    static void Teardown();
    static bool CanLoad(const std::string &name);
    static Model* Load(const std::string &name);

protected:
    ModelMS3D() {}
    virtual ~ModelMS3D() {}

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
