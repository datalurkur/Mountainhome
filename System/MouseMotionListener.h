/*
 *  MouseMotionListener.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 5/1/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MOUSEMOTIONLISTENER_H_
#define _MOUSEMOTIONLISTENER_H_

/*! \brief Encapsulates information associated with changes in the mouse's position.
    \author Brent Wilson
    \date 4/30/07 */
class MouseMotionEvent {
public:
    MouseMotionEvent(int absX, int absY, int relX, int relY);
    ~MouseMotionEvent();
    
    int absX();
    int absY();

    int relX();
    int relY();
    
private:
    int _absX;
    int _absY;
    int _relX;
    int _relY;
};

/*! \brief Interface that allows objects to be informed of mouse button events.
    \author Brent Wilson
    \date 4/30/07 */
class MouseMotionListener {
public:
    virtual void mouseMoved(MouseMotionEvent *event) = 0;
};

#endif
