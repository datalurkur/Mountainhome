/*
 *  Content.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/FileSystem.h>

#include "Content.h"

#include "ResourceGroupManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "FontManager.h"

ResourceGroupManager *Content::_resourceGroupManager = NULL;
MaterialManager *Content::_materialManager = NULL;
TextureManager *Content::_textureManager = NULL;
ShaderManager *Content::_shaderManager = NULL;
ModelManager *Content::_modelManager = NULL;
FontManager *Content::_fontManager = NULL;

MaterialManager * Content::GetMaterialManager() { return _materialManager; }
TextureManager * Content::GetTextureManager()   { return _textureManager;  }
ShaderManager * Content::GetShaderManager()     { return _shaderManager;   }
ModelManager * Content::GetModelManager()       { return _modelManager;    }
FontManager * Content::GetFontManager()         { return _fontManager;     }

void Content::Initialize() {
    _resourceGroupManager = new ResourceGroupManager();
    _textureManager = new TextureManager(_resourceGroupManager);
    _shaderManager = new ShaderManager(_resourceGroupManager);
    _modelManager = new ModelManager(_resourceGroupManager, _textureManager);
    _materialManager = new MaterialManager(_resourceGroupManager, _shaderManager, _textureManager);
    _fontManager = new FontManager(_resourceGroupManager, _materialManager, _textureManager, _shaderManager);
}

void Content::AddResourceDir(const std::string &dir) {
    std::string formattedDir = dir;
    FileSystem::FormatPath(formattedDir);
    _resourceGroupManager->addResourceLocation(formattedDir, true);
}

template <> Material * Content::GetOrLoad(const std::string &name) { return GetMaterialManager()->getOrLoadResource(name); }
template <> Material * Content::Get(const std::string &name)       { return GetMaterialManager()->getCachedResource(name); }
template <> Texture * Content::GetOrLoad(const std::string &name) { return GetTextureManager()->getOrLoadResource(name); }
template <> Texture * Content::Get(const std::string &name)       { return GetTextureManager()->getCachedResource(name); }
template <> Shader * Content::GetOrLoad(const std::string &name) { return GetShaderManager()->getOrLoadResource(name); }
template <> Shader * Content::Get(const std::string &name)       { return GetShaderManager()->getCachedResource(name); }
template <> Model * Content::GetOrLoad(const std::string &name) { return GetModelManager()->getOrLoadResource(name); }
template <> Model * Content::Get(const std::string &name)       { return GetModelManager()->getCachedResource(name); }
template <> Font * Content::GetOrLoad(const std::string &name) { return GetFontManager()->getOrLoadResource(name); }
template <> Font * Content::Get(const std::string &name)       { return GetFontManager()->getCachedResource(name); }
