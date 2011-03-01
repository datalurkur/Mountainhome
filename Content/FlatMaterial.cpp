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
"}\n";
    std::string flatFrag =
"uniform vec4 color;\n"
"void main() {\n"
"    gl_FragColor = color;\n"
"}\n";

    ShaderGLSL *flat = new ShaderGLSL(flatVert, "", flatFrag);
    sManager->registerResource("flat", flat);
}

FlatMaterial::FlatMaterial(): _color(1,0,1,1) {
    init();
}

FlatMaterial::FlatMaterial(const Vector4 &color): _color(color) {
    init();
}

void FlatMaterial::init() {
    setShader(Content::Get<Shader>("flat"));

    // Point the color parameter at our local color variable. We only need to do this once.
    setShaderParameter("color", &_color);
}

void FlatMaterial::setColor(const Vector4 &color) {
    _color = color;
}

const Vector4 & FlatMaterial::getColor() {
    return _color;
}
