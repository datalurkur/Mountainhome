/*
 *  Mountainhome.h
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#ifndef _MOUNTAINHOME_H_
#define _MOUNTAINHOME_H_
#include <Base/Singleton.h>
#include <Engine/DefaultCore.h>

class Mountainhome : public DefaultCore, public Singleton<Mountainhome> {
public:
    static const std::string GameStateID;
    static Window *GetWindow();

public:
    virtual void keyPressed(KeyEvent *event);
    virtual void setup(va_list args);

protected:
    Mountainhome();
    virtual ~Mountainhome();
    template <class T> friend class Singleton;

    // To allow MountainhomeBinding to call registerState
    friend class MountainhomeBinding;

};
#endif
