/*
 *  Material.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Base/FileSystem.h>

#include "Material.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Shader.h"

Material::Factory::Factory(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager):
PTreeResourceFactory<Material>(rManager), _shaderManager(sManager), _textureManager(tManager) { }

Material::Factory::~Factory() {
}

bool Material::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    return ext == "material";
}

Material* Material::Factory::load(const std::string &name) {
    Material *mat = new Material();

    // Set the shader.
    std::string shaderFile = _ptree.get<std::string>("shader", "");
    if(shaderFile.length() > 0) {
        mat->setShader(_shaderManager->getOrLoadResource(shaderFile));
    }

    // Add any textures.
    for(int c = 0; c < 8; c++) {
        std::stringstream textureKey;
        textureKey << "texture" << c;
        std::string textureFile = _ptree.get<std::string>(textureKey.str(), "");
        if (textureFile.length() > 0) {
            mat->setTexture(_textureManager->getOrLoadResource(textureFile), c);
        }
    }

    // Set the basic color values.
    mat->setTransparent(_ptree.get<int>("transparent", 0));
    mat->setAmbient(_ptree.get<Vector4>("ambient",Vector4(1.0f)));
    mat->setDiffuse(_ptree.get<Vector4>("diffuse",Vector4(1.0f)));
    mat->setColor(  _ptree.get<Vector4>("color",  Vector4(1.0f)));

    return mat;
}

Material::Material(): _color(1.0), _ambient(0.0), _diffuse(0.0), _materialShader(NULL), _transparent(false) {
    int c;
    for(c=0; c < 8; c++) {
        _texture[c] = NULL;
    }
}

Material::~Material() {}

void Material::setTransparent(bool value) {
    _transparent = value;
}

void Material::setColor(Real r, Real g, Real b, Real a) {
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
	_color[3] = a;
}

void Material::setColor(Vector4 color) { _color = color; }

void Material::setAmbient(Real r, Real g, Real b, Real a) {
	_ambient[0] = r;
	_ambient[1] = g;
	_ambient[2] = b;
}

void Material::setAmbient(Vector4 ambient) { _ambient = ambient; }

void Material::setDiffuse(Real r, Real g, Real b, Real a) {
	_diffuse[0] = r;
	_diffuse[1] = g;
	_diffuse[2] = b;
}

void Material::setDiffuse(Vector4 diffuse) { _diffuse = diffuse; }

void Material::setTexture(Texture *t, int level) {
	_texture[level] = t;
}

void Material::enableMaterial() const {
	glColor4f(_color[0], _color[1], _color[2], _color[3]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*)&_diffuse);

    if (_transparent) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
    
    if(_materialShader) {
        _materialShader->on();
    }

	// Set up textures
	if (_texture[0]) { 
        int c;
        for(c=7; c >= 0; c--) {
            if(_texture[c]) {
                (_texture[c])->bindAndEnable(c);
            }
        }
    }
	else { glDisable(GL_TEXTURE_2D); }
}

void Material::disableMaterial() const {
    if(_materialShader) {
        _materialShader->off();
    }
    
    if(_texture[0]) {
        int c;
        for(c=7; c >= 0; c--) {
            if(_texture[c]) {
                (_texture[c])->releaseAndDisable(c);
            }
        }
    }
}

bool Material::getTransparent() const {
    return _transparent;
}

Texture* Material::getTexture(int level) const {
	return _texture[level];
}

Vector4* Material::getAmbient() const {
	return (Vector4*)&_ambient;
}

Vector4* Material::getDiffuse() const {
	return (Vector4*)&_diffuse;
}

Vector4* Material::getColor() const {
	return (Vector4*)&_color;
}
