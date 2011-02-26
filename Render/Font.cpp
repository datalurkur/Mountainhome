/*
 *  Font.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <stdarg.h>

#include "Buffer.h"

#include "Render.h"
#include "FontManager.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Font.h"
#include "File.h"

Font::Font(Material *mat):
    _glyph(NULL),
    _material(mat),
    _defaultColor(1, 1, 1, 1),
    _texWidth(1),
    _texHeight(1),
    _cellWidth(0),
    _cellHeight(0),
    _fontHeight(0),
    _fontDescent(0),
    _fontAscent(0),
    _lineSkip(0)
{}

Font::~Font() {}

void Font::setDefaultColor(const Color4 &color) {
    _defaultColor = color;
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

int Font::getVisibleCharacterCount(const char * buffer) {
    int count = 0;
    for (;buffer != NULL; buffer++) {
        if (*buffer > 32) { count++; }
    }

    return 0;
}

int Font::splitTextAt(const string &buffer, int maxWidth) {
    const char *cBuffer = buffer.c_str();
    int index = 0, size = 0;

    for(const char* current = cBuffer; *current; current++) {
        size += _fontWidth[*current];
        if(size >= maxWidth) {
            return index;
        }
        index++;
    }

    return -1;
}

#define VPRINTF(bufname, bufsize, format) \
    do { \
        bufname = new char[(bufsize)]; \
        va_list args; \
        va_start(args, (format)); \
        std::vsnprintf(bufname, (bufsize), (format), args); \
        va_end(args); \
    } while(0)

FontRenderable * Font::print(const char *format, ...) {
    char *buffer;
    VPRINTF(buffer, 1024, format);
    return printBuffer(_defaultColor, buffer);
}

FontRenderable * Font::print(const Color4 &color, const char *format, ...) {
    char *buffer;
    VPRINTF(buffer, 1024, format);
    return printBuffer(color, buffer);
}

#undef VPRINTF

FontRenderable * Font::printBuffer(const Color4 &color, const char *buffer) {
    int count = getVisibleCharacterCount(buffer);
    IVector2 *positions = new IVector2[4 * count];
    Vector2  *texcoords = new  Vector2[4 * count];

    fillInVertices(positions, texcoords, count, buffer);

    // Create the renderable.
    VertexArray *elementArray = new VertexArray();
    elementArray->setPositionBuffer(new PositionBuffer(GL_STATIC_DRAW, GL_INT,   2, 4 * count, positions));
    elementArray->setTexCoordBuffer(0, new TexCoordBuffer(GL_STATIC_DRAW, GL_FLOAT, 2, 4 * count, texcoords));
    RenderOperation *renderOp = new RenderOperation(QUADS, elementArray);

    delete[] positions;
    delete[] texcoords;

    return new FontRenderable(renderOp, _material, color, getWidth(buffer), getHeight(), buffer);
}

void Font::fillInVertices(IVector2 *positions, Vector2 *texcoords, int count, const std::string &buffer) {
    IVector2 currentPos(0, 0);
    int currentChar;

    std::vector<std::string> lines;
    tokenize <std::vector<std::string> > (buffer, "\n", lines);
    for (int i = 0; i < lines.size(); i++) {
        for (int j = 0; j < lines[i].size(); j++) {
            char currentASCII = lines[i].c_str()[j];
            int currentWidth = _fontWidth[currentASCII];

            if (currentASCII > 32) {
                // Fill in positions.
                positions[currentChar * 4 + 0] = IVector2(currentPos.x,                currentPos.y + _fontDescent);
                positions[currentChar * 4 + 1] = IVector2(currentPos.x + currentWidth, currentPos.y + _fontDescent);
                positions[currentChar * 4 + 2] = IVector2(currentPos.x + currentWidth, currentPos.y + _fontAscent );
                positions[currentChar * 4 + 3] = IVector2(currentPos.x,                currentPos.y + _fontAscent );

                // Fill in texcoords.
                int cellRow = currentASCII / 16;
                int cellCol = currentASCII % 16;

                Real uDiff = _cellWidth  / Real(_texWidth);
                Real vDiff = _cellHeight / Real(_texHeight);

                Real u1 = cellRow * uDiff, u2 = u1 + uDiff;
                Real v1 = cellCol * vDiff, v2 = v1 + vDiff;

                texcoords[currentChar * 4 + 0] = Vector2(u1, v1);
                texcoords[currentChar * 4 + 1] = Vector2(u2, v1);
                texcoords[currentChar * 4 + 2] = Vector2(u2, v2);
                texcoords[currentChar * 4 + 3] = Vector2(u1, v2);

                // Update the current visible character index we're on.
                currentChar++;
            }

            currentPos.x += currentWidth;
        }
        
        currentPos.y -= _lineSkip;
    }

    // Should have filled in the max number of characters by now.
    ASSERT(currentChar == count);
}
