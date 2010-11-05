/*
 *  MHCore.h
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#ifndef _MHCORE_H_
#define _MHCORE_H_
#include <Base/Singleton.h>
#include <Engine/DefaultCore.h>

class RubyState;

/*! MHCore is the main application class which inherits from the Engine's
 *  DefaultCore, making it entirely central to the application. It provides the entry
 *  point for the application and handles all initial setup and final shutdown and
 *  delegates where the execution flow goes as it is handed over by the engine (update,
 *  display, any form of input, etc... all go through this object first). This class also
 *  defines ruby binding for itself, which gives us access to the root of the application
 *  from ruby.
 * \note SetupBindings installs a global $mhcore variable in ruby.
 * \fixme Sadly all of the state interaction code here is a duplicate of what we have in
 *  RubyState. I'm not sure what the best way to get around this is, though (perhaps just
 *  reference RubyState methods in here? Maybe we can do some scary casting nonsense). For
 *  now I'm just leaving in the duplicate code. */
class MHCore : public DefaultCore {
public:
    MHCore();
    virtual ~MHCore();

    virtual void keyPressed(KeyEvent *event);
    virtual void innerLoop(int elapsed);

protected:

};

#endif
