/*
 *  Font.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <stdarg.h>

#include "Render.h"
#include "FontManager.h"
#include "RenderTarget.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Font.h"
#include "File.h"

Font::Font(Shader *shader): _textureId(0), _fontLists(0), _fontShader(shader), _originLocation(BottomLeft),
_color(1,1,1,1), _texWidth(1), _texHeight(1), _cellWidth(0), _cellHeight(0),
_fontHeight(0), _fontDescent(0), _fontAscent(0), _lineSkip(0) {}

Font::~Font() {
    glDeleteTextures(1, &_textureId);
}

void Font::setColor(Real r, Real g, Real b, Real a) {
    setColor(Color4(r, g, b, a));
}

void Font::setColor(const Color4 &color) {
    _color = color;
}

void Font::setupGL(int windowWidth, int windowHeight) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 0, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    if (_fontShader) { _fontShader->on(); }
    
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    
    glColor4fv(_color.array);
}

void Font::revertGL() {
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glPopAttrib();
    if (_fontShader) { _fontShader->off(); }
}

int Font::getHeight() {
    return _fontAscent + _fontDescent;
}

int Font::getWidth(const string &buffer) {
    return getWidth(buffer.c_str());
}

int Font::getWidth(const char* buffer) {
    int result = 0;
    for (const char* current = buffer; *current; current++) {
        result += _fontWidth[*current];
    }

    return result;
}

void Font::print(int x, int y, int windowWidth, int windowHeight, const char* format, ...) {
    int bufferLength(1024);
    char buffer[bufferLength];

    //Handle string formatting
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, 1024, format, args);
    va_end(args);

    printBuffer(x, y, windowWidth, windowHeight, buffer);
}

void Font::printBuffer(int x, int y, int windowWidth, int windowHeight, const char* buffer) {
    setupGL(windowWidth, windowHeight);

    switch(_originLocation) {
        case TopRight:
            glTranslatef(x - getWidth(buffer), y - getHeight(), 0);
            break;
        case TopMiddle:
            glTranslatef(x - (getWidth(buffer) >> 1), y - getHeight(), 0);
            break;
        case TopLeft:
            glTranslatef(x, y - getHeight(), 0);
            break;
        case MiddleRight:
            glTranslatef(x - getWidth(buffer), y - (getHeight() >> 1), 0);
            break;
        case Middle:
            glTranslatef(x - (getWidth(buffer) >> 1), y - (getHeight() >> 1), 0);
            break;
        case MiddleLeft:
            glTranslatef(x, y - (getHeight() >> 1), 0);
            break;
        case BottomRight:
            glTranslatef(x - getWidth(buffer), y, 0);
            break;
        case BottomMiddle:
            glTranslatef(x - (getWidth(buffer) >> 1), y, 0);
            break;
        default: // BottomLeft
            glTranslatef(x, y, 0);
    }
    

    char *current = strchr(buffer, '\n');
    while(current) {
        printLine(buffer, current - buffer);
        buffer = current + 1;
        current = strchr(buffer, '\n');
    }
    printLine(buffer, strlen(buffer));
    revertGL();
}

void Font::printLine(const char* buffer, int length) {
    glPushMatrix();
    glListBase(_fontLists);
    glCallLists(length, GL_UNSIGNED_BYTE, buffer);
    glPopMatrix();
    glTranslatef(0, _lineSkip * -1, 0);
}

void Font::renderGlyphToScreen(Real x, Real y) {
    renderGlyphToScreen(x, y, _texWidth, _texHeight);
}

void Font::renderGlyphToScreen(Real x, Real y, Real w, Real h) {
    // Setup the general OGL state.
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    // Put it on a black background.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    glColor3f(1.0, 1.0, 1.0);

    // Setup the texture.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    // Draw the glyph.
    glBegin(GL_QUADS); {
        glTexCoord2f(0, 0); glVertex2f(x    , y    );
        glTexCoord2f(1, 0); glVertex2f(x + w, y    );
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x    , y + h);
    } glEnd();

    // And go back to how things were.
    glPopAttrib();
}

void Font::buildLists() {
    unsigned char currentASCII = 0;
    Real uDiff = _cellWidth  / Real(_texWidth);
    Real vDiff = _cellHeight / Real(_texHeight);
    _fontLists = glGenLists(256);

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            glNewList(_fontLists + currentASCII, GL_COMPILE);
            switch (currentASCII) {
                case  9: glTranslatef(_fontWidth[32] * 4, 0.0f, 0.0f); break;
                case 32: glTranslatef(_fontWidth[32], 0.0f, 0.0f);     break;
                default: drawLetter(j * uDiff, i * vDiff, _fontWidth[currentASCII]);
            }
            glEndList();
            currentASCII++;
        }
    }
}

void Font::drawLetter(Real u1, Real v1, int charWidth) {
    Real u2 = u1 + (charWidth / Real(_texWidth));
    Real v2 = v1 + ((_fontHeight)/ Real(_texHeight));
    glBegin(GL_QUADS); {
        glTexCoord2f(u1, v1); glVertex2i(0, _fontDescent);
        glTexCoord2f(u2, v1); glVertex2i(charWidth, _fontDescent);
        glTexCoord2f(u2, v2); glVertex2i(charWidth, _fontAscent);
        glTexCoord2f(u1, v2); glVertex2i(0, _fontAscent);
    } glEnd();
    glTranslatef(charWidth, 0.0f, 0.0f);
}
