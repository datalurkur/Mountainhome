/*
 *  FontTTF.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ShaderManager.h"
#include "FontTTF.h"

#include <Base/ResourceGroupManager.h>
#include <Base/FileSystem.h>

FontTTF::Factory::Factory(ResourceGroupManager *rManager, ShaderManager *sManager):
PTreeResourceFactory<Font>(rManager), _shaderManager(sManager) {
    if (TTF_Init() == -1) {
        THROW(InternalError, "Could not init TTF: " << TTF_GetError());
    }

    addRequiredKey("font");
    addRequiredKey("size");
}

FontTTF::Factory::~Factory() {
    TTF_Quit();
}

bool FontTTF::Factory::canLoad(const std::string &name) {
    std::string ttfFile, ext;
    ttfFile = _ptree.get<std::string>("font");
    FileSystem::ExtractExtension(ttfFile, ext);
    return ext == "ttf";
}

Font* FontTTF::Factory::load(const std::string &name) {
    std::string filename = getPathFromKey("font");
    Shader *shader = _shaderManager->getOrLoadResource("font.shader");
    return new FontTTF(shader, filename , _ptree.get<int>("size"));
}

FontTTF::FontTTF(Shader *shader, const std::string &fontPath, int size): Font(shader) {
    initFont(fontPath, size);
}

FontTTF::~FontTTF() {}

void FontTTF::initFont(const std::string &filename, int size) {
    TTF_Font *font = TTF_OpenFont(filename.c_str(), size);
    if (!font) {
        THROW(InternalError, "Could not open " << filename << " : " << TTF_GetError());
    }

    // TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    loadMetrics(font);
    createGlyph(font);
    buildLists();

    TTF_CloseFont(font);        
}

void FontTTF::loadMetrics(TTF_Font *font) {
    _cellHeight = TTF_FontHeight(font);
    _cellWidth = 0;
    
    int x1, x2, y1, y2, advance;
    for( int i = 0; i < 256; ++i ) {
        TTF_GlyphMetrics(font, static_cast<Uint16>(i), &x1, &x2, &y1, &y2, &advance);
        _fontWidth[i] = advance;
        if(advance > _cellWidth) {
            _cellWidth = advance;
        }
    }
    
    _fontAscent = TTF_FontAscent(font);
    _fontDescent = TTF_FontDescent(font);
    _fontHeight = _fontAscent - _fontDescent;
    _lineSkip = TTF_FontLineSkip(font);
}

void FontTTF::createGlyph(TTF_Font *font) {
    _texWidth = _texHeight = 1;
    while (_texWidth < _cellWidth * 16) { _texWidth = _texWidth << 1; }
    while (_texHeight < _cellHeight * 16) { _texHeight = _texHeight << 1; }
    
    int numTexels = _texHeight * _texWidth;
    unsigned char *texels = new unsigned char[numTexels];
    memset(texels, 0, numTexels);
    
    fontToGlyph(font, texels);
    glyphToGL(texels);
    
    delete[] texels;
}

void FontTTF::glyphToGL(unsigned char *texels) {
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _texWidth, _texHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texels);
}

///\note This is the code for if I turn the textures back around for SDL:
///      texels[((_texHeight - 1 - (yPos + h)) * _texWidth) + (xPos + w)] = a;

void FontTTF::fontToGlyph(TTF_Font *font, unsigned char *texels) {
    SDL_Color white = { 255, 255, 255, 255 };
    int r, g, b, a, xPos, yPos;
    int currentASCII = -1;
    
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            currentASCII++;
            if (currentASCII == 0) { continue; }
            char letter[2] = {currentASCII, 0};
            
            xPos = j * _cellWidth;
            yPos = i * _cellHeight;
            SDL_Surface *renderedLetter = TTF_RenderText_Blended(font, letter, white);

            for (int w = 0; w < renderedLetter->w; w++) {
                for (int h = 0; h < renderedLetter->h; h++) {
                    SDL_GetPixel(renderedLetter, w, renderedLetter->h - 1 - h, r, g, b, a);
                    texels[((yPos + h) * _texWidth) + (xPos + w)] = a;
                }
            }
            
            SDL_FreeSurface(renderedLetter);
        }
    }
}
