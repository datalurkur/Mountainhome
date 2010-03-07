/*
 *  Mouse.h
 *  Base
 *
 *  Created by Brent Wilson on 11/11/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_
#include <Base/Singleton.h>

// Note that this class may safely be a singleton as it contains no state.
class Mouse : public Singleton<Mouse> {
public:
    enum MouseButton {
        NONE = 0,
        BUTTON_LEFT,
        BUTTON_MIDDLE,
        BUTTON_RIGHT,
        BUTTON_WHEELUP,
        BUTTON_WHEELDOWN
    };

    bool isButtonDown(int key);
    void pressButton(int button, int x, int y);
    void getMousePos(int &x, int &y);
    void setMousePos(int x, int y);

protected:
       Mouse();
    virtual ~Mouse();

    template <class T> friend class Singleton;
};

#endif
