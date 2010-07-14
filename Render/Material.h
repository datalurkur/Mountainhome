/*
 *  Material.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include <Base/Math3D.h>
#include <Base/Vector.h>
#include <Render/Texture.h>
#include <Base/PTreeResourceFactory.h>

///\todo Look into making a parent class for everything that needs set/get ambient/diffuse/specular
class Shader;
class ShaderManager;
class TextureManager;

class Material {
public:
    class Factory : public PTreeResourceFactory<Material> {
    public:
        Factory(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager);
        virtual ~Factory();

        bool canLoad(const std::string &args);
        Material* load(const std::string &args);
    private:
        ShaderManager *_shaderManager;
        TextureManager *_textureManager;
    };

public:
    Material();
    virtual ~Material();

    void setTransparent(bool value);
	void setColor(Real r, Real g, Real b, Real a);
    void setColor(Vector4 color);
    void setAmbient(Real r, Real g, Real b, Real a = 1.0f);
    void setAmbient(Vector4 ambient);
    void setDiffuse(Real r, Real g, Real b, Real a = 1.0f);
    void setDiffuse(Vector4 diffuse);
	void setTexture(Texture *t, int level = 0);
    
    void enableMaterial() const;
    void disableMaterial() const;

    void applyLighting(bool val);
    void setShader(Shader *shader) { _materialShader = shader; }
    Shader *getShader() { return _materialShader; }

    bool getTransparent() const;
	Texture *getTexture(int level = 0) const;
	Vector4 *getColor() const;
	Vector4 *getAmbient() const;
	Vector4 *getDiffuse() const;
	
private:
	Vector4 _color;
	Vector4 _ambient;
	Vector4 _diffuse;
	Texture *_texture[8];
    Shader *_materialShader;

    bool _transparent;
    bool _applyLighting;
};

#endif
