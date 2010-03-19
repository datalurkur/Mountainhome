/*
 *  Font.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FONT_H_
#define _FONT_H_
#include <Base/Vector.h>
#include "GL_Helper.h"

/*! \brief Allows the user to render text to the screen based on .ttf files.
    \todo Move loading logic into the FontManager. <????>
    \todo Make it so ttf font is not needed here (just in manager). <????>
    \todo Remove SDL/GL_Helper.h */
class Shader;
class Font {
public:
    enum OriginLocation {
        BottomLeft,
        BottomMiddle,
        BottomRight,
        MiddleRight,
        TopRight,
        TopMiddle,
        TopLeft,
        MiddleLeft,
        Middle
    };

public:
    int getHeight();
    int getWidth(const char* buffer);
    int getWidth(const std::string &buffer);

    void print(const char* buffer, int x, int y);
    void print(int x, int y, const char* format, ...);

    void setColor(const Color4 &color);
    void setColor(Real r, Real g, Real b, Real a);
    void renderGlyphToScreen(Real x, Real y, Real w, Real h);

protected:
    template <typename Resource, typename IdType> friend class ResourceManager;

    Font();
    virtual ~Font();

    void buildLists();
    void drawLetter(Real u, Real v, int charWidth);

    void setupGL();
    void revertGL();
    void printLine(const char* buffer, int length);

protected:
    GLuint _textureId;
    GLuint _fontLists;
    Shader *_fontShader;
    int    _originLocation;
    Color4 _color;

protected:
    int _texWidth;
    int _texHeight;
    int _cellWidth;
    int _cellHeight;

    int _fontWidth[256];
    int _fontHeight;
    int _fontDescent;
    int _fontAscent;
    int _lineSkip;

};

#endif
