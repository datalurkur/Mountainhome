/*
 *  FontTTF.h
 *  Mountainhome
 *
 *  Created by loch on 3/18/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _FONTTTF_H_
#define _FONTTTF_H_
#include "FontManager.h"
#include "SDL_Helper.h"

class FontTTF : public Font {
public:
    class Factory : public ResourceFactory<Font> {
    public:
        Factory();
        virtual ~Factory();
        bool canLoad(const std::string &args);
        Font* load(const std::string &args);
    };

protected:
    FontTTF(const std::string &fontPath, int size);
    virtual ~FontTTF();

    void loadMetrics(TTF_Font *font);
    void createGlyph(TTF_Font *font);
    void glyphToGL(unsigned char *texels);
    void fontToGlyph(TTF_Font *font, unsigned char *texels);
    void initFont(const std::string &fontPath, int size);

};

#endif
