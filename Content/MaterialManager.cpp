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
    registerResource("black", new BasicMaterial(Vector4(0.0, 0.0, 0.0, 1.0)));
	registerResource("red",   new BasicMaterial(Vector4(1.0, 0.0, 0.0, 1.0)));
	registerResource("green", new BasicMaterial(Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource("blue",  new BasicMaterial(Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource("grey",  new BasicMaterial(Vector4(0.5, 0.5, 0.5, 1.0)));
	registerResource("white", new BasicMaterial(Vector4(1.0, 1.0, 1.0, 1.0)));

    registerResource("red-lit",   new BasicMaterial(Vector4(0.3,  0.15, 0.15, 1.0), Vector4(1.0, 0.0, 0.0, 1.0)));
    registerResource("green-lit", new BasicMaterial(Vector4(0.15, 0.3,  0.15, 1.0), Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource("blue-lit",  new BasicMaterial(Vector4(0.15, 0.15, 0.3,  1.0), Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource("grey-lit",  new BasicMaterial(Vector4(0.15, 0.15, 0.15, 1.0), Vector4(0.5, 0.5, 0.5, 1.0)));
    registerResource("white-lit", new BasicMaterial(Vector4(0.3,  0.3,  0.3,  1.0), Vector4(1.0, 1.0, 1.0, 1.0)));
}

MaterialManager::~MaterialManager() {}
