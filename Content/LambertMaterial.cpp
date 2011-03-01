/*
 *  LambertMaterial.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "Content.h"
#include "LambertMaterial.h"
#include "ShaderManager.h"
#include "ShaderGLSL.h"

void LambertMaterial::Init(ShaderManager *sManager) {
    std::string lembertVert =
"uniform vec4 ambient, diffuse;\n"
"\n"
"varying vec4 diffuseTerm, ambientTerm;\n"
"varying vec3 normal, lightDirection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = ftransform();\n"
"\n"
"    // Calculate lighting. For directional lights, the position is actually the direction.\n"
"    lightDirection = normalize(gl_LightSource[0].position.xyz);\n"
"\n"
"    diffuseTerm =\n"
"        diffuse * gl_LightSource[0].diffuse;\n"
"    ambientTerm =\n"
"        ambient * gl_LightSource[0].ambient +\n"
"        ambient * gl_LightModel.ambient;\n"
"\n"
"    // And do the normal, finally.\n"
"    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
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

    ShaderGLSL *lambert = new ShaderGLSL(lembertVert, "", lambertFrag);
    sManager->registerResource("lambert", lambert);
}

LambertMaterial::LambertMaterial() {
    init();
}

LambertMaterial::LambertMaterial(const Vector4 &ambient, const Vector4 &diffuse) {
    init();
    setAmbient(ambient);
    setDiffuse(diffuse);
}

void LambertMaterial::init() {
    setShader(Content::Get<Shader>("lambert"));

    // Point the color parameters at our local variables. We only need to do this once.
    setShaderParameter("ambient", &_ambient);
    setShaderParameter("diffuse", &_diffuse);
}

void LambertMaterial::setAmbient(const Vector4 &ambient) {
    _ambient = ambient;
}

const Vector4 & LambertMaterial::getAmbient() {
    return _ambient;
}

void LambertMaterial::setDiffuse(const Vector4 &diffuse) {
    _diffuse = diffuse;
}

const Vector4 & LambertMaterial::getDiffuse() {
    return _diffuse;
}
