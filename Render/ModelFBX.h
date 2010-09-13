/*
 *  ModelFBX.h
 *  Engine
 *
 *  Created by Andrew Jean on on 9/12/10.
 *
 */

#ifndef _MODELFACTORYFBX_H_
#define _MODELFACTORYFBX_H_
#include <Base/ResourceManager.h>
#include "Model.h"
#include <fbxsdk.h>

class ModelFBX : public Model {
public:
    class Factory : public ResourceFactory<Model> {
    public:
        Factory(ResourceGroupManager *manager);
        virtual ~Factory();
        bool canLoad(const std::string &args);
        Model* load(const std::string &args);

    private:
        KFbxSdkManager* _sdkManager;
        KFbxImporter* _importer;
    };

protected:
    ModelFBX();
    virtual ~ModelFBX() {}

};

#endif
