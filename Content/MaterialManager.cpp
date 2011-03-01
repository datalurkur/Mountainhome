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

#include "LambertMaterial.h"
#include "FlatMaterial.h"

MaterialManager::MaterialManager(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager) {
    registerFactory(new MaterialFactory(rManager, sManager, tManager));

    LambertMaterial::Init(sManager);
    FlatMaterial::Init(sManager);

    // Create some default materials.
    registerResource("black", new FlatMaterial(Vector4(0.0, 0.0, 0.0, 1.0)));
	registerResource("red",   new FlatMaterial(Vector4(1.0, 0.0, 0.0, 1.0)));
	registerResource("green", new FlatMaterial(Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource("blue",  new FlatMaterial(Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource("grey",  new FlatMaterial(Vector4(0.5, 0.5, 0.5, 1.0)));
	registerResource("white", new FlatMaterial(Vector4(1.0, 1.0, 1.0, 1.0)));

    registerResource("red-lit",   new LambertMaterial(Vector4(0.3,  0.15, 0.15, 1.0), Vector4(1.0, 0.0, 0.0, 1.0)));
    registerResource("green-lit", new LambertMaterial(Vector4(0.15, 0.3,  0.15, 1.0), Vector4(0.0, 1.0, 0.0, 1.0)));
    registerResource("blue-lit",  new LambertMaterial(Vector4(0.15, 0.15, 0.3,  1.0), Vector4(0.0, 0.0, 1.0, 1.0)));
    registerResource("grey-lit",  new LambertMaterial(Vector4(0.15, 0.15, 0.15, 1.0), Vector4(0.5, 0.5, 0.5, 1.0)));
    registerResource("white-lit", new LambertMaterial(Vector4(0.3,  0.3,  0.3,  1.0), Vector4(1.0, 1.0, 1.0, 1.0)));

    Material *font = new Material();
    font->setShader(sManager->getCachedResource("font"));
    registerResource("font", font);
}

MaterialManager::~MaterialManager() {}
