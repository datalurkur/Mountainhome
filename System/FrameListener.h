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

class FrameListener {
public:
	virtual int frameStarted(int elapsed) = 0;
};

#endif

/*****************************************************************************************
*  File: $Id: FrameListener.h,v 1.1 2007/05/02 08:54:31 bmw2863 Exp $
*
*  Revisions:
*        $Log: FrameListener.h,v $
*        Revision 1.1  2007/05/02 08:54:31  bmw2863
*        Numerous minor changes were made. The most notable was that viewport
*        now can hold multiple render sources. When a rendersource is added it
*        is informed of who it's owner is. If a source is deleted it, it is
*        responsible for informing the viewport.
*
*
****************************************************************************************/
