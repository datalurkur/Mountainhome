/*
 *  ModelManager.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/06/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_
#include <Base/ResourceManager.h>
#include <Base/Singleton.h>
#include "Model.h"

class ModelManager : public ResourceManager<Model> {
public:
    ModelManager(ResourceGroupManager *manager);
    ~ModelManager();

//	//Load a model.
//	static Model* Load(const char* directory, const char* filename);
//	//Loads an animated Model.
//	static ModelAnimated* LoadAnimated(const char* directory, const char* filename);
//	//Loads a static Model.
//	static ModelStatic* LoadStatic(const char* directory, const char* filename);
//
//private:
//	std::map<const char*, Model*> model_map;		//The list of Model names and their 
//                                                //associated Models
//
//	//Gets the type of model from the given filename.
//	void getModelType(const char* filename, char* type);
//	//Loads an animated Model.
//	ModelAnimated* loadAnimated(const char* directory, const char* filename);
//	//Loads a static Model.
//	ModelStatic* loadStatic(const char* directory, const char* filename);
//	//Deletes a Model based on its filename.
//	void unloadModel(const char* filename);
//	//Delete all Modeles in memory.
//	void freeModels();

};

#endif
