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
#include "Renderable.h"

/*! A special renderer containing addition data specific to dealing with fonts. */
class FontRenderable : public Renderable {
public:
    FontRenderable(
        RenderOperation *op,
        Material *material,
        const Color4 &color,
        int width, int height,
        const char * text)
    :
        Renderable(op, material),
        _color(color),
        _width(width),
        _height(height),
        _text(text)
    {
        setShaderParameter("color", &_color);
    }

    virtual ~FontRenderable() {
        delete[] _text;
    }

    int getHeight() { return _height; }
    int getWidth() { return _width; }
    const char * getText() { return _text; }

private:
    Color4 _color;
    int _width, _height;
    const char *_text;

};

class RenderTarget;

/*! \brief Allows the user to render text to the screen based on .ttf files.
 *  \todo Move loading logic into the FontManager. <????>
 *  \todo Make it so ttf font is not needed here (just in manager). <????>
 *  \todo Remove SDL/GL_Helper.h
 *  \todo Get Font to use Renderables and Materials.  */
class Shader;
class Font {
public:
    int getHeight();

    int getWidth(const char *buffer);

    int getWidth(const std::string &buffer);

    int splitTextAt(const std::string &buffer, int maxWidth);

    Texture * getGlyphTexture();

    void setDefaultColor(const Color4 &color);

    FontRenderable * print(const char *format, ...);

    FontRenderable * print(const Color4 &color, const char *format, ...);

protected:
    template <typename Resource> friend class ResourceManager;

    Font(Material *mat);
    virtual ~Font();

    FontRenderable * printBuffer(const Color4 &color, const char *buffer);

    void fillInVertices(IVector2 *positions, Vector2 *texcoords, int count, const std::string &buffer);

    int getVisibleCharacterCount(const char * buffer);

protected:
    Texture *_glyph;
    Material *_material;
    Color4 _defaultColor;

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
