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
    /*! These are done without templates because they avoid the complications of calling
     *  template functions from GDB. Using templates results in this sort of nonsense:
     *
     *      (gdb) call Content::GetNameOf<Material>(this)
     *      There is no field named GetNameOf<Material>
     *      (gdb) call Content::GetNameOf(this)
     *      There is no field named GetNameOf
     *
     *  Using identical names gives us:
     *
     *      (gdb) call Content::GetNameOf(this)
     *      non-unique member `GetNameOf' requires type instantiation
     *
     *  which I do not feel like dealing with, especially not since they all need to be
     *  specialized anyways.
     * \todo Fix this. */
    static std::string GetNameOfMaterial(const Material *resource);
    static std::string GetNameOfTexture(const Texture *resource);
    static std::string GetNameOfShader(const Shader *resource);
    static std::string GetNameOfModel(const Model *resource);
    static std::string GetNameOfFont(const Font *resource);

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

template <typename T> T * Content::GetOrLoad(const std::string &name) {
    THROW(NotImplementedError, "This is not a valid Content type!");
}

template <typename T> T * Content::Get(const std::string &name) {
    THROW(NotImplementedError, "This is not a valid Content type!");
}

template <> Material *  Content::GetOrLoad(const std::string &name);
template <> Material *  Content::Get(const std::string &name);

template <> Texture *   Content::GetOrLoad(const std::string &name);
template <> Texture *   Content::Get(const std::string &name);

template <> Shader *    Content::GetOrLoad(const std::string &name);
template <> Shader *    Content::Get(const std::string &name);

template <> Model *     Content::GetOrLoad(const std::string &name);
template <> Model *     Content::Get(const std::string &name);

template <> Font *      Content::GetOrLoad(const std::string &name);
template <> Font *      Content::Get(const std::string &name);

#endif
