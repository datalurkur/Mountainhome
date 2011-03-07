/*
 *  Shader.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Shader.h"

Shader::Shader() {}
Shader::~Shader() {}

void Shader::bindAttributesToChannel(const std::vector<std::string> &names) {
    for (int i = 0; i < names.size(); i++) {
        bindAttributeToChannel(names[i], i);
    }
}

void Shader::setParameters(const ShaderParameterMap &params) {
    ShaderParameterMap::const_iterator itr;
    for (itr = params.begin(); itr != params.end(); itr++) {
        setParameter(itr->first, itr->second);
    }
}
