/*
 *  FlatMaterial.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "ShaderGLSL.h"
#include "ShaderManager.h"
#include "FlatMaterial.h"
#include "Content.h"

void FlatMaterial::Init(ShaderManager *sManager) {
    std::string flatVert =
"void main() {\n"
"    gl_Position = ftransform();\n"
"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
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
}

FlatMaterial::FlatMaterial(): _color(1, 1, 1, 1), _texture(NULL) {
    update();
}

FlatMaterial::FlatMaterial(const Vector4 &color, Texture *tex): _color(color), _texture(tex) {
    update();
}

void FlatMaterial::update() {
    if (_texture) {
        setShader(Content::Get<Shader>("flat-textured"));
    } else {
        setShader(Content::Get<Shader>("flat"));
    }

    // Point the color parameter at our local color variable. We only need to do this once.
    setShaderParameter("color", &_color);

    // If the _texture is null, this needs to be called to clear the parameter.
    setShaderParameter("texture", _texture);
}

void FlatMaterial::setColor(const Vector4 &color) {
    _color = color;
}

const Vector4 & FlatMaterial::getColor() {
    return _color;
}

void FlatMaterial::setTexture(Texture *tex) {
    _texture = tex;
    update();
}

const Texture * FlatMaterial::getTexture() {
    return _texture;
}
