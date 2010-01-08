/*
 *  EventPump.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _EVENTPUMP_H_
#define _EVENTPUMP_H_
#include <list>

class KeyListener;
class WindowListener;
class MouseButtonListener;
class MouseMotionListener;

/*! \brief Encapsulates the system's native event pump.
    \author Brent Wilson
    \date 4/4/07 */
class EventPump {
public:
       EventPump();
    virtual ~EventPump();

    void processEvents();
    void clearListeners();

    void addWindowListener(WindowListener *listener);
    void addMouseButtonListener(MouseButtonListener *listener);
    void addMouseMotionListener(MouseMotionListener *listener);
    void addKeyListener(KeyListener *listener);

private:
    std::list<WindowListener*> _windowListeners;
    std::list<MouseButtonListener*> _mouseButtonListeners;
    std::list<MouseMotionListener*> _mouseMotionListeners;
    std::list<KeyListener*> _keyListeners;

};

#endif
