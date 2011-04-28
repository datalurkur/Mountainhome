/*
 *  MaterialManager.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MaterialFactory.h"
#include "MaterialManager.h"
#include "ShaderManager.h"

#include "BasicMaterial.h"

MaterialManager::MaterialManager(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager) {
    registerFactory(new MaterialFactory(rManager, sManager, tManager));

    BasicMaterial::Init(sManager);

    // Create some default materials.
    registerResource(new BasicMaterial("black", Vector4(0.0, 0.0, 0.0, 1.0)));
	registerResource(new BasicMaterial("red", Vector4(1.0, 0.0, 0.0, 1.0)));
	registerResource(new BasicMaterial("green", Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource(new BasicMaterial("blue", Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource(new BasicMaterial("grey", Vector4(0.5, 0.5, 0.5, 1.0)));
	registerResource(new BasicMaterial("white", Vector4(1.0, 1.0, 1.0, 1.0)));

    registerResource(new BasicMaterial("red-lit", Vector4(0.3,  0.15, 0.15, 1.0), Vector4(1.0, 0.0, 0.0, 1.0)));
    registerResource(new BasicMaterial("green-lit", Vector4(0.15, 0.3,  0.15, 1.0), Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource(new BasicMaterial("blue-lit", Vector4(0.15, 0.15, 0.3,  1.0), Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource(new BasicMaterial("grey-lit", Vector4(0.15, 0.15, 0.15, 1.0), Vector4(0.5, 0.5, 0.5, 1.0)));
    registerResource(new BasicMaterial("white-lit", Vector4(0.3,  0.3,  0.3,  1.0), Vector4(1.0, 1.0, 1.0, 1.0)));
}

MaterialManager::~MaterialManager() {}

void MaterialManager::registerResource(Material *resource) {
    ResourceManager<Material>::registerResource(resource->getName(), resource);
}
