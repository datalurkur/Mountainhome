/*
 *  MaterialManager.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MaterialManager.h"

MaterialManager::MaterialManager(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager) {
    registerFactory(new Material::Factory(rManager, sManager, tManager));

    // Create some default materials.
    Material *grey = new Material();
    grey->setColor(0.5, 0.5, 0.5, 1.0);

    Material *white = new Material();
	white->setColor(1.0, 1.0, 1.0, 1.0);
    white->setAmbient(1.0, 1.0, 1.0, 1.0);
    white->setDiffuse(1.0, 1.0, 1.0, 1.0);

	Material *red = new Material();
	red->setColor(1.0, 0.0, 0.0, 1.0);
    red->setAmbient(1.0, 0.0, 0.0, 1.0);
    red->setDiffuse(1.0, 0.0, 0.0, 1.0);

	Material *blue = new Material();
	blue->setColor(0.0, 0.0, 1.0, 1.0);
	blue->setAmbient(0.0, 0.0, 1.0, 0.0);
	blue->setDiffuse(0.0, 0.0, 1.0, 1.0);

    registerResource("grey",  grey );
	registerResource("white", white);
	registerResource("red",   red  );
	registerResource("blue",  blue );
}

MaterialManager::~MaterialManager() {}
