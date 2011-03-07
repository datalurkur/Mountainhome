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
#include "PTreeResourceFactory.h"
#include "FontManager.h"
#include "SDL_Helper.h"

class ShaderManager;
class FontTTF : public Font {
public:
    class Factory : public PTreeResourceFactory<Font> {
    public:
        Factory(ResourceGroupManager *rManager, MaterialManager *mManager, TextureManager *tManager);
        virtual ~Factory();

        bool canLoad(const std::string &args);
        Font* load(const std::string &args);
    private:
        MaterialManager *_materialManager;
        TextureManager *_textureManager;

    };

protected:
    FontTTF(Material *mat, const std::string &fontPath, int size, TextureManager *manager);
    virtual ~FontTTF();

    void loadMetrics(TTF_Font *font);
    void createGlyph(TTF_Font *font, const std::string &name, TextureManager *manager);
    void fontToGlyph(TTF_Font *font, unsigned char *texels);

};

#endif
