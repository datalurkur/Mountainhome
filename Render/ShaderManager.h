/*
 *  ShaderManager.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_
#include "ResourceManager.h"
#include "Singleton.h"
#include "Shader.h"

/*! \brief Provides the functionality for Shader caching and Loading. */
class ShaderManager : public ResourceManager<Shader> {
public:
    ShaderManager(ResourceGroupManager *manager);
    ~ShaderManager();

};

#endif
