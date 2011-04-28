/*
 *  BasicMaterial.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/2/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "Content.h"
#include "BasicMaterial.h"
#include "ShaderManager.h"
#include "ShaderGLSL.h"

// Not certain why, but the gl_TextureMatrix breaks this really hard :/
//"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"

void BasicMaterial::Init(ShaderManager *sManager) {
    std::string flatVert =
"void main() {\n"
"    gl_Position = ftransform();\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"}\n";
    std::string flatFrag =
"uniform vec4 color;\n"
"void main() {\n"
"    gl_FragColor = color;\n"
"}\n";
    std::string texuredflatFrag =
"uniform sampler2D texture;\n"
"uniform vec4 color;\n"
"void main() {\n"
"    gl_FragColor = color * texture2D(texture, gl_TexCoord[0].st);\n"
"}\n";

    sManager->registerResource("flat", new ShaderGLSL(flatVert, "", flatFrag));
    sManager->registerResource("flat-textured", new ShaderGLSL(flatVert, "", texuredflatFrag));

    std::string lembertVert =
"uniform vec4 ambient, diffuse;\n"
"\n"
"varying vec4 diffuseTerm, ambientTerm;\n"
"varying vec3 normal, lightDirection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"\n"
"    // Calculate lighting. For directional lights, the position is actually the direction.\n"
"    lightDirection = normalize(vec3(gl_LightSource[0].position));\n"
"\n"
"    diffuseTerm =\n"
"        diffuse * gl_LightSource[0].diffuse;\n"
"    ambientTerm =\n"
"        ambient * gl_LightSource[0].ambient +\n"
"        ambient * gl_LightModel.ambient;\n"
"\n"
"    // And do the normal, finally.\n"
"    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
"\n"
"    // Handle texturing."
"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n" // THIS NEEDS TO BE HERE. I DON'T KNOW WHY. IT MAKES NO SENSE AND ANGERS ME.
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"}\n";

    std::string lambertFrag =
"varying vec4 diffuseTerm, ambientTerm;\n"
"varying vec3 normal, lightDirection;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 eyeNormal = normalize(normal);\n"
"    float NdotL    = max(dot(eyeNormal, lightDirection), 0.0);\n"
"    gl_FragColor   = ambientTerm + diffuseTerm * NdotL;\n"
"}\n";

    std::string texturedLambertFrag =
"varying vec4 diffuseTerm, ambientTerm;\n"
"varying vec3 normal, lightDirection;\n"
"uniform sampler2D texture;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 eyeNormal = normalize(normal);\n"
"    float NdotL    = max(dot(eyeNormal, lightDirection), 0.0);\n"
"    vec4 lighting  = ambientTerm + diffuseTerm * NdotL;\n"
"    gl_FragColor   = lighting * texture2D(texture, gl_TexCoord[0].st);\n"
//"    gl_FragColor   = vec4(gl_TexCoord[0].st, 0, 1);\n"
//"    gl_FragColor   = vec4(1, 0, 0, 1);\n"
"}\n";

    sManager->registerResource("lambert", new ShaderGLSL(lembertVert, "", lambertFrag));
    sManager->registerResource("lambert-textured", new ShaderGLSL(lembertVert, "", texturedLambertFrag));
}

BasicMaterial::BasicMaterial():
    _ambient(1, 1, 1, 1),
    _diffuse(1, 1, 1, 1),
    _texture(NULL),
    _lighting(false)
{
    updateTechnique();
}

BasicMaterial::BasicMaterial(const std::string &name):
    Material(name),
    _ambient(1, 1, 1, 1),
    _diffuse(1, 1, 1, 1),
    _texture(NULL),
    _lighting(false)
{
    updateTechnique();
}

BasicMaterial::BasicMaterial(const std::string &name, const Vector4 &diffuse, Texture *tex):
    Material(name),
    _ambient(1, 1, 1, 1),
    _diffuse(diffuse),
    _texture(tex),
    _lighting(false)
{
    updateTechnique();
}

BasicMaterial::BasicMaterial(const std::string &name, const Vector4 &ambient, const Vector4 &diffuse, Texture *tex):
    Material(name),
    _ambient(ambient),
    _diffuse(diffuse),
    _texture(tex),
    _lighting(true)
{
    updateTechnique();
}

void BasicMaterial::updateTechnique() {
    if (_lighting) {
        // Choose the appropriate lighting shader.
        setShader(Content::Get<Shader>(_texture ? "lambert-textured" : "lambert"));

        // Setup out color parameters.
        setShaderParameter("ambient", &_ambient);
        setShaderParameter("diffuse", &_diffuse);

        clearShaderParameter("color");

        // If the _texture is null, this needs to be called to clear the parameter.
        setShaderParameter("texture", _texture);    
    } else {
        // Choose the appropriate unlit shader.
        setShader(Content::Get<Shader>(_texture ? "flat-textured" : "flat"));

        // Setup out color parameter.
        setShaderParameter("color", &_diffuse);

        clearShaderParameter("ambient");
        clearShaderParameter("diffuse");

        // If the _texture is null, this needs to be called to clear the parameter.
        setShaderParameter("texture", _texture);
    }
}

void BasicMaterial::setAmbient(const Vector4 &ambient) {
    _ambient = ambient;
}

const Vector4 & BasicMaterial::getAmbient() const {
    return _ambient;
}

void BasicMaterial::setDiffuse(const Vector4 &diffuse) {
    _diffuse = diffuse;
}

const Vector4 & BasicMaterial::getDiffuse() const {
    return _diffuse;
}

void BasicMaterial::setTexture(Texture *tex) {
    _texture = tex;
    updateTechnique();
}

const Texture * BasicMaterial::getTexture() const {
    return _texture;
}

void BasicMaterial::setLightingEnabled(bool enable) {
    _lighting = enable;
    updateTechnique();
}

bool BasicMaterial::getLightingEnabled() const {
    return _lighting;
}

