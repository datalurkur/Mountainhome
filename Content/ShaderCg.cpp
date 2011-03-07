/*
 *  ShaderCg.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/22/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ShaderCg.h"

ShaderCg::Factory::Factory(ResourceGroupManager *manager): PTreeResourceFactory<Shader>(manager) {}
ShaderCg::Factory::~Factory() {}

bool ShaderCg::Factory::canLoad(const std::string &args) {
    return false;
}

Shader* ShaderCg::Factory::load(const std::string &args) {
    return NULL;
}
