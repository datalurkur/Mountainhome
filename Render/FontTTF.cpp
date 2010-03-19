/*
 *  FontTTF.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "FontTTF.h"
#include <Base/ResourceGroupManager.h>
#include <Base/FileSystem.h>

FontTTF::Factory::Factory() {
    if (TTF_Init() == -1) {
        THROW(InternalError, "Could not init TTF: " << TTF_GetError());
    }
}

FontTTF::Factory::~Factory() {
    TTF_Quit();
}

bool FontTTF::Factory::canLoad(const FontArgsList &args) {
    std::string ext;
    FileSystem::ExtractExtension(args.name, ext);
    return ext == "ttf";
}

Font* FontTTF::Factory::load(const FontArgsList &args) {
    Font *font = new FontTTF(args.name, args.size);
    return font;
}

FontTTF::FontTTF(const std::string &fontPath, int size): Font() {
    initFont(fontPath, size);
}

FontTTF::~FontTTF() {}

void FontTTF::initFont(const std::string &fontName, int size) {
    std::string filename = ResourceGroupManager::Get()->findResource(fontName);
    TTF_Font *font = TTF_OpenFont(filename.c_str(), size);
    if (!font) {
        Error("Could not open " << fontName << " : " << TTF_GetError());
        return;
    }

    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
