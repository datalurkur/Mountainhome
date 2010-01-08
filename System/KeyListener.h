/*
 *  KeyboardListener.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/5/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _KEYLISTENER_H_
#define _KEYLISTENER_H_

/*! \brief Encapsulates information associated with changes in the keyboard's state.
    \author Brent Wilson
    \date 4/5/07 */
class KeyEvent {
public:
    KeyEvent(int k, int m);
    ~KeyEvent();

    int key();
    int modifier();

private:
    int _key;
    int _mod;
};

/*! \brief Interface that allows objects to be informed of keyboard events.
    \author Brent Wilson
    \date 4/5/07 */
class KeyListener {
public:
    /*! This method is called automatically whenever a key is clicked.
     * \param event A KeyEvent representing the key clicked event. */
    virtual void keyTyped(KeyEvent *event) = 0;

    /*! This method is called automatically whenever a key is pressed.
     * \param event A KeyEvent representing the key pressed event. */
    virtual void keyPressed(KeyEvent *event) = 0;

    /*! This method is called automatically whenever a key is released.
     * \param event A KeyEvent representing the key released event. */
    virtual void keyReleased(KeyEvent *event) = 0;
};

#endif
