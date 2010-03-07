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

/*! The key event contains all of the information needed to describe some use driven
 *  keyboard event. This includes the key that was pressed/released/typed and all
 *  modifiers that were pressed during the event.
 * \brief Encapsulates information associated with changes in the keyboard's state.
 * \seealso Keyboard for key and modifer definitions */
class KeyEvent {
public:
    /*! Creates a new KeyEvent with the given key value and modifier values.
     * \seealso Keyboard for key and modifer definitions. */
    KeyEvent(int k, int m);

    /*! D'tor */
    ~KeyEvent();

    /*! Gets the key code for the event.
     * \seealso Keyboard for key and modifer definitions. */
    int key();

    /*! Gets the bit array representing current modifers.
     * \seealso Keyboard for key and modifer definitions. */
    int modifier();

private:
    int _key; /*!< The key associated with the event. */
    int _mod; /*!< The modifers associated with the event. */

};

/*! This is an pure virtual interface that can be registered with the EventPump, allowing
 *  the registered object to be informed of any and all keyboard state changes in the form
 *  of a KeyEvent. The event pump supports three different kinds of events: typed,
 *  pressed, and released. pressed and released are fairly obvious. Typed happens when
 *  a user presses and releases a key very quickly. Note that in this case, all three
 *  events will be fired.
 * \brief Interface that allows objects to be informed of keyboard events. */
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
