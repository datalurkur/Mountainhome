/*
 *  MouseButtonListener.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/5/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MOUSEBUTTONLISTENER_H_
#define _MOUSEBUTTONLISTENER_H_

/*! \brief Encapsulates information associated with changes in the mouse's button state.
    \author Brent Wilson
    \date 4/5/07 */
class MouseButtonEvent {
public:
    MouseButtonEvent(int button, int x, int y);
    ~MouseButtonEvent();

    int x();
    int y();
    int button();

private:
    int _button;
    int _x;
    int _y;
};

/*! \brief Interface that allows objects to be informed of mouse button events.
    \author Brent Wilson
    \date 4/5/07 */
class MouseButtonListener {
public:
    virtual void mouseClicked(MouseButtonEvent *event) = 0;
    virtual void mousePressed(MouseButtonEvent *event) = 0;
    virtual void mouseReleased(MouseButtonEvent *event) = 0;
};

#endif
