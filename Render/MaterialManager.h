/*
 *  MaterialManager.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELMANAGER_H
#define _MODELMANAGER_H
#include <Base/ResourceManager.h>
#include <Base/Singleton.h>
#include "Material.h"

class MaterialManager : public ResourceManager<Material> {
public:
    MaterialManager(ResourceGroupManager *manager);
    ~MaterialManager();

};

#endif
