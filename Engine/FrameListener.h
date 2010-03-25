/*
 *  FrameListener.h
 *  Engine
 *
 *  Created by Brent Wilson on 5/2/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FRAMELISTENER_H_
#define _FRAMELISTENER_H_

/*! This is a listener interface allowing users to register for notice when a new frame
 *  has started. */
class FrameListener {
public:
    FrameListener() {}
    virtual ~FrameListener() {}
	virtual int frameStarted(int elapsed) = 0;
};

#endif
