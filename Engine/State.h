#ifndef _STATE_H_
#define _STATE_H_
#include <Base/Base.h>
#include "InputListener.h"

class ParentState;

class State : public InputListener {
public:
    /*! State c'tor */
    State();

    /*! State d'tor */
    virtual ~State();

    /*! Draw is automatically called whenever the state is active.
     * \note Eventually, this may be moved into a separate thread from update. */
    virtual void draw() = 0;

    /*! Update is automatically called whenever the state is active.
     * \param elapsed The elapsed time since the last update call in seconds. */
    virtual void update(int elapsed) = 0;

    /*! Called when the current state is active and a key is typed.
     * \seealso KeyListener::keyTyped */
    virtual void keyTyped(KeyEvent *event);

    /*! Called when the current state is active and a key is pressed.
     * \seealso KeyListener::keyPressed */
    virtual void keyPressed(KeyEvent *event);

    /*! Called when the current state is active and a key is released.
     * \seealso KeyListener::keyReleased */
    virtual void keyReleased(KeyEvent *event);

    /*! Called when the current state is active and a mouse button is clicked.
     * \seealso MouseButtonListener::mouseClicked */
    virtual void mouseClicked(MouseButtonEvent *event);

    /*! Called when the current state is active and a mouse button is pressed.
     * \seealso MouseButtonListener::mousePressed */
    virtual void mousePressed(MouseButtonEvent *event);

    /*! Called when the current state is active and a mouse button is released.
     * \seealso MouseButtonListener::mouseReleased */
    virtual void mouseReleased(MouseButtonEvent *event);

    /*! Called when the current state is active and a mouse button is moved.
     * \seealso MouseMotionListener::mouseMoved */
    virtual void mouseMoved(MouseMotionEvent *event);

protected:

    /*! Any State sub-class is expected to implement a setup method. It is called whenever
     *  the state is made active or pushed onto the state state.
     * \param args A space separated set of arguments that may contain context sensitive
     *  data that is up to the sub class to interpret. */
    virtual void setup(va_list args) = 0;

    /*! Much like setup, this function must be implemented by subclasses. It is called
     *  whenever the state stop being active. */
    virtual void teardown() = 0;

protected:
    friend class ParentState;
    void notifyRegistered(ParentState *parent, const std::string &name);

    ParentState *_parent;
    std::string _name;

};

#endif
