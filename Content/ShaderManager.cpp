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
"varying vec4 diffuseTerm, ambientGlobalTerm, ambientTerm;\n"
"varying vec3 normal, lightDir, halfVector;\n"
"varying float dist;\n"
"\n"
"void main()\n"
"{\n"
"    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
"\n"
"    /* these are the new lines of code to compute the light's direction */\n"
"    vec3 delta = vec3(gl_LightSource[0].position - (gl_ModelViewMatrix * gl_Vertex));\n"
"    lightDir = normalize(delta);\n"
"    dist = length(delta);\n"
"\n"
"    halfVector = normalize(gl_LightSource[0].halfVector.xyz);\n"
"\n"
"    /* Compute the diffuse, ambient and globalAmbient terms */\n"
"    diffuseTerm = diffuse * gl_LightSource[0].diffuse;\n"
"\n"
"    /* The ambient terms have been separated since one of them suffers attenuation */\n"
"    ambientTerm = ambient * gl_LightSource[0].ambient;\n"
"    ambientGlobalTerm = ambient * gl_LightModel.ambient;\n"
"\n"
"    gl_Position = ftransform();\n"
"}\n";

    std::string lambertFrag =
"varying vec4 diffuseTerm, ambientGlobalTerm, ambientTerm;\n"
"varying vec3 normal, lightDir, halfVector;\n"
"varying float dist;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 n, halfV, viewV, ldir;\n"
"    float NdotL, NdotHV;\n"
"    vec4 color = ambientGlobalTerm;\n"
"    float att;\n"
"\n"
"    /* a fragment shader can't write a varying variable, hence we need\n"
"    a new variable to store the normalized interpolated normal */\n"
"    n = normalize(normal);\n"
"\n"
"    /* compute the dot product between normal and normalized lightdir */\n"
"    NdotL = max(dot(n,normalize(lightDir)),0.0);\n"
"\n"
"    if (NdotL > 0.0) {\n"
"\n"
"        att = 1.0 / (gl_LightSource[0].constantAttenuation +\n"
"                gl_LightSource[0].linearAttenuation * dist +\n"
"                gl_LightSource[0].quadraticAttenuation * dist * dist);\n"
"        color += att * (diffuseTerm * NdotL + ambientTerm);\n"
"\n"     
"        halfV = normalize(halfVector);\n"
"        NdotHV = max(dot(n,halfV),0.0);\n"
"        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * \n"
"                        pow(NdotHV,gl_FrontMaterial.shininess);\n"
"    }\n"
"\n"
"    gl_FragColor = color;\n"
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
