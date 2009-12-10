/*
 *  WindowListener.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/5/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _WINDOWLISTENER_H_
#define _WINDOWLISTENER_H_

/*! \brief Interface that allows objects to be informed of window events.
    \author Brent Wilson
    \date 4/5/07 */
class WindowListener {
public:
    virtual void windowClosing() = 0;
    virtual void windowResized(int w, int h) = 0;
};

#endif
