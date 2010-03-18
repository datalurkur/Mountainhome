/*
 *  FontManager.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_
#include "ResourceManager.h"
#include "Singleton.h"
#include "Font.h"

struct FontArgsList {
    FontArgsList(std::string n, int s): name(n), size(s) {}
    std::string name;
    int size;
};

/*! \brief Provides the functionality for Font caching and Loading. */
class FontManager : public ResourceManager<Font, FontArgsList>, public Singleton<FontManager> {
protected:
    FontManager();
    virtual ~FontManager();

    template <class T> friend class Singleton;
};

#endif
