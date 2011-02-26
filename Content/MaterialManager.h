/*
 *  MaterialManager.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MATERIALMANAGER_H
#define _MATERIALMANAGER_H
#include <Base/Singleton.h>
#include <Render/Material.h>
#include "ResourceManager.h"

class MaterialManager : public ResourceManager<Material> {
public:
    MaterialManager(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager);
    ~MaterialManager();

};

#endif
