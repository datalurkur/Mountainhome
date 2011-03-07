/*
 *  MaterialFactory.h
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MATERIALFACTORY_H_
#define _MATERIALFACTORY_H_
#include <Render/Material.h>
#include "PTreeResourceFactory.h"

class MaterialFactory : public PTreeResourceFactory<Material> {
public:
    MaterialFactory(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager);
    virtual ~MaterialFactory();

    bool canLoad(const std::string &args);
    Material* load(const std::string &args);

private:
    ShaderParameter * parseShaderParameter(const std::string &input);

    void setGenericParameters(Material *mat);

    template <int Size>
    ShaderParameter * parseFloat(const std::vector<std::string> &tokens);

    template <int Size>
    ShaderParameter * parseInt(const std::vector<std::string> &tokens);

private:
    ShaderManager *_shaderManager;
    TextureManager *_textureManager;
    std::list<std::string> _keywords;
};

template <int Size>
ShaderParameter * MaterialFactory::parseFloat(const std::vector<std::string> &tokens) {
    // We MUST malloc here so ShaderParameter can clean up after us.
    Vector<Size> *value = (Vector<Size>*)malloc(sizeof(Vector<Size>));
    for (int i = 0; i < Size; i++) {
        (*value)[i] = strtod(tokens[i+1].c_str(), (char **)NULL);
    }

    ShaderParameter *result = new ShaderParameter();
    result->setData(value, 1, true);
    return result;
}

template <int Size>
ShaderParameter * MaterialFactory::parseInt(const std::vector<std::string> &tokens) {
    // We MUST malloc here so ShaderParameter can clean up after us.
    IVector<Size> *value = (IVector<Size>*)malloc(sizeof(IVector<Size>));
    for (int i = 0; i < Size; i++) {
        (*value)[i] = strtol(tokens[i+1].c_str(), (char **)NULL, 10);
    }

    ShaderParameter *result = new ShaderParameter();
    result->setData(value, 1, true);
    return result;
}

#endif
