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

/*! \brief Provides the functionality for Shader caching and Loading.
    \author Brent Wilson
    \date 4/22/07 */
class ShaderManager : public ResourceManager<Shader>, public Singleton<ShaderManager> {
protected:
    ShaderManager();
    ~ShaderManager();

    template <class T> friend class Singleton;

public:
    Shader* getOrLoadResource(const std::string &vertFilename, const std::string &fragFilename);
    Shader* loadResource(const std::string &name, const std::string &vertFilename, const std::string &fragFilename);
    Shader* loadFromText(const std::string &name, const std::string &vertString, const std::string &fragString);

private:
    void textFromFile(const std::string &filename, std::string &result) const;
    void loadVertex(const std::string &vertString, Shader* shader);
};

#endif
