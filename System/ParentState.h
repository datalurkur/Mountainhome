#ifndef _PARENTSTATE_H_
#define _PARENTSTATE_H_
#include "State.h"

/*! The ParentState is a delegation state. It can have State objects and other ParentState
 *  objects register as child states. The ParentState keeps track of all of its children
 *  and maintains a state stack. The top of the stack is considered the "active state" and
 *  all input and update events are delegated to that child. Child states may be pushed on
 *  and popped off of the stack. As children are placed on the stack and taken off, their
 *  setup and teardown methods are called. When this class's teardown method is called,
 *  all states on the stack are popped off. */
class ParentState : public State {
public:
    /*! ParentState c'tor */
    ParentState();

    /*! ParentState d'tor */
    virtual ~ParentState();

    /*! Pops all states off of the stack (calling teardown on each) and looks for a child
     *  with the named child. If a match is made, its setup method is called and it is
     *  placed on the stack.
     * \note If no match is made, setActiveState is called on the parent state. If there
     *  is no parent state, an exception is thrown.
     * \param key The name of the state to change to and the arguments to pass to the
     *  state's setup function. The first word is used as the state name and the rest of
     *  the string is used as the argument list. */
    void setActiveState(const std::string &key, ...);

    /*! This pushes a new state onto the state stack. The current state is left on the
     *  stack, allowing the pushed state to be popped off, restoring the old state. The
     *  new state has its setup method called.
     * \note If no match is made, pushState is called on the parent state. If there is no
     *  parent state, an exception is thrown.
     * \param key The name of the state to change to and the arguments to pass to the
     *  state's setup function. The first word is used as the state name and the rest of
     *  the string is used as the argument list. */
    void pushState(const std::string &key, ...);

    /*! Pops the top state off of the stack, calling its teardown function. If the pop
     *  action results in an empty state stack, popState is called on the parent.
     *  This will happen until the pop results in a non empty state stack.
     * \note If the pop results in an empty state stack and there is no parent, an
     *  exception is thrown. */
    void popState();

public:
    /*! Calls update on the currently active state
     * \seealso State::update */
    virtual void update(int elapsed);

    /*! Delegates to the currently active state.
     * \seealso State::keyTyped */
    virtual void keyTyped(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::keyPressed */
    virtual void keyPressed(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::keyReleased */
    virtual void keyReleased(KeyEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseMoved */
    virtual void mouseMoved(MouseMotionEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseClicked */
    virtual void mouseClicked(MouseButtonEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mousePressed */
    virtual void mousePressed(MouseButtonEvent *event);

    /*! Delegates to the currently active state.
     * \seealso State::mouseReleased */
    virtual void mouseReleased(MouseButtonEvent *event);

protected:
    /*! Blank implementation.
     * \seealso State::setup */
    virtual void setup(va_list args) {}

    /*! Blank implementation.
     * \seealso State::teardown */
    virtual void teardown() {}

protected:
    /*! Gets the state that is on the top of the state stack.
     * \return The state on the top of the stack or NULL if there is none. */
    State *getActiveState();

    /*! This registers a state as a possible child and gives the string used to access it.
     * \param s The actual state.
     * \param key The name used to access the state. */
    void registerState(State *s, const std::string &key);

    /*! This clears the active state stack and deletes all registered child states */
    void clearStates();

    /*! This clears the active state stack. */
    void clearStack();

private:
    void setActiveState(const std::string &key, va_list args);
    void pushState(const std::string &s, bool setState, va_list args);
    void pushState(const std::string &key, va_list args);
    void popState(bool cascade);

private:
    std::map<std::string, State *> _states;
    std::list<State *> _active;

};

#endif
