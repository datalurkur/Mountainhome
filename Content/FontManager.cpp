/*
 *  FontManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "FontManager.h"
#include "FontTTF.h"
#include "ShaderManager.h"
#include "ShaderGLSL.h"

FontManager::FontManager(ResourceGroupManager *rManager, MaterialManager *mManager, TextureManager *tManager, ShaderManager *sManager) {
    registerFactory(new FontTTF::Factory(rManager, mManager, tManager));

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
    sManager->registerResource("font", font);
}

FontManager::~FontManager() {}
