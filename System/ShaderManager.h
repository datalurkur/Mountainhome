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
    Shader* loadResource(const string &vertFilename, const string &fragFilename);
    Shader* loadFromFile(const string &name, const string &vertFilename, const string &fragFilename);
    Shader* loadFromText(const string &name, const string &vertString, const string &fragString);

private:
    void textFromFile(const string &filename, string &result) const;
    void loadVertex(const string &vertString, Shader* shader);
};

#endif
