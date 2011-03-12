/*
 *  Content.h
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CONTENT_H_
#define _CONTENT_H_
#include <Base/Exception.h>
#include <string>

class ResourceGroupManager;
class MaterialManager;
class TextureManager;
class ShaderManager;
class ModelManager;
class FontManager;

class Material;
class Texture;
class Shader;
class Model;
class Font;

class Content {
public:
    template <typename T> static T * GetOrLoad(const std::string &name);

    template <typename T> static T * Get(const std::string &name);

    static MaterialManager * GetMaterialManager();

    static TextureManager * GetTextureManager();

    static ShaderManager * GetShaderManager();

    static ModelManager * GetModelManager();

    static FontManager * GetFontManager();

    static void Initialize();

    static void AddResourceDir(const std::string &dir);
    
private:
    static ResourceGroupManager *_resourceGroupManager;
    static MaterialManager *_materialManager;
    static TextureManager *_textureManager;
    static ShaderManager *_shaderManager;
    static ModelManager *_modelManager;
    static FontManager *_fontManager;

};

template <typename T> T * GetOrLoad(const std::string &name) {
    THROW(NotImplementedError, "This is not a valid Content type!");
}

template <typename T> T * Get(const std::string &name) {
    THROW(NotImplementedError, "This is not a valid Content type!");
}

template <> Material * GetOrLoad(const std::string &name);
template <> Material * Get(const std::string &name);

template <> Texture * GetOrLoad(const std::string &name);
template <> Texture * Get(const std::string &name);

template <> Shader * GetOrLoad(const std::string &name);
template <> Shader * Get(const std::string &name);

template <> Model * GetOrLoad(const std::string &name);
template <> Model * Get(const std::string &name);

template <> Font * GetOrLoad(const std::string &name);
template <> Font * Get(const std::string &name);

#endif
