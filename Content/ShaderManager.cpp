/*
 *  ShaderManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "ShaderManager.h"
#include "ShaderGLSL.h"
#include "ShaderCg.h"

ShaderManager::ShaderManager(ResourceGroupManager *manager) {
    registerFactory(new ShaderGLSL::Factory(manager));
    registerFactory(new ShaderCg::Factory(manager));

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
    registerResource("lambert", lambert);

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
    registerResource("flat", flat);

    std::string fontVert =
"uniform vec4 color;\n"
"void main() {\n"
"	gl_FrontColor = color;\n"
"	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
"	gl_Position = ftransform();\n"
"}\n";

    std::string fontFrag =
"uniform sampler2D glyph;\n"
"void main() {\n"
"	gl_FragColor = gl_Color;\n"
"	gl_FragColor.a = texture2D(glyph, gl_TexCoord[0].st).a;\n"
"}\n";

    ShaderGLSL *font = new ShaderGLSL(fontVert, "", fontFrag);
    registerResource("font", font);

}

ShaderManager::~ShaderManager() {}
