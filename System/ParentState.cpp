#include "ParentState.h"

ParentState::ParentState(): State() {}

ParentState::~ParentState() {
    clearStates();
}

void ParentState::clearStates() {
    clearStack();
    clear_map(_states);
}

void ParentState::clearStack() {
    Trace("Clearing state stack for '" << _name << "'");

    while (getActiveState()) {
        popState(false);
    }
}

void ParentState::setActiveState(const std::string &state, ...) {
    va_list args;

    clearStack();
    va_start(args, state);
    setActiveState(state, args);
    va_end(args);
}

void ParentState::pushState(const std::string &state, ...) {
    va_list args;

    va_start(args, state);
    pushState(state, args);
    va_end(args);
}

void ParentState::setActiveState(const std::string &state, va_list args) {
    clearStack();
    pushState(state, true, args);
}

void ParentState::pushState(const std::string &state, va_list args) {
    pushState(state, false, args);
}

void ParentState::pushState(const std::string &state, bool setState, va_list args) {
    // Actually perform the state switch.
    if (_states.find(state) != _states.end()) {
        if (setState) { Info("Setting '" << _name << "'s child to '" << state << "'");
        } else { Info("pushing '" << state << "' onto '" << _name << "'"); }

        _active.push_front(_states[state]);
        getActiveState()->setup(args);
    } else {
        // This group doesn't have the named state, so check the parent.
        if (_parent) {
            if (setState) {
                _parent->setActiveState(state, args);
            } else {
                _parent->pushState(state, args);
            }
        } else {
//            OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "No state found named '" +
//                state + "'.", "ParentState::pushState");
        }
    }
}

void ParentState::popState() {
    popState(true);
}

void ParentState::popState(bool cascade) {
    if (getActiveState()) {
        Info("Popping '" << getActiveState()->_name << "' off of '" << _name << "'");

        // If the top state is a parent state, we need to clear its state stack.
        ParentState *parent = dynamic_cast<ParentState*>(getActiveState());
        if (parent) {
            parent->clearStack();
        }

        // Teardown the state and cascade to this state's parent if needed.
        getActiveState()->teardown();
        _active.pop_front();
        if (!getActiveState() && cascade) {
            if (_parent) {
                _parent->popState(true);
            } else {
                RAISE(InvalidStateError, "No previously pushed state was found. "
                    "Possible inconsistency in '" + _name + "'.");
            }
        }
    } else {
        RAISE(InvalidStateError, "No previously pushed state was found. Possible "
            "inconsistency in '" + _name + "'.");
    }
}

State *ParentState::getActiveState() {
    return _active.size() ? _active.front() : NULL;
}

void ParentState::registerState(State *s, const std::string &state) {
    Info("Registering new child (" << state << ") for " << _name << ".");

    if (_states.find(state) != _states.end()) {
        RAISE(DuplicateItemError, "State named '" + state + "' already exists!");
    }

    _states[state] = s;
    s->notifyRegistered(this, state);
}

void ParentState::update(int elapsed) {
    if (getActiveState()) {
        getActiveState()->update(elapsed);
    }
}

void ParentState::keyTyped(KeyEvent *event) {
    getActiveState()->keyPressed(event);
}

void ParentState::keyPressed(KeyEvent *event) {
    getActiveState()->keyPressed(event);
}

void ParentState::keyReleased(KeyEvent *event) {
    getActiveState()->keyReleased(event);
}

void ParentState::mouseMoved(MouseMotionEvent *event) {
    getActiveState()->mouseMoved(event);
}

void ParentState::mouseClicked(MouseButtonEvent *event) {
    getActiveState()->mouseClicked(event);
}

void ParentState::mousePressed(MouseButtonEvent *event) {
    getActiveState()->mousePressed(event);
}

void ParentState::mouseReleased(MouseButtonEvent *event) {
    getActiveState()->mouseReleased(event);
}
