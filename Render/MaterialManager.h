/*
 *  MaterialManager.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELMANAGER_H
#define _MODELMANAGER_H
#include <Base/SingleFileManager.h>
#include <Base/Singleton.h>
#include "Material.h"

class MaterialManager : public SingleFileManager<Material>, public Singleton<MaterialManager> {
protected:
    MaterialManager();
    ~MaterialManager();

    template <class T> friend class Singleton;

};

#endif
