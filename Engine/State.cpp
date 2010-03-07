#include "State.h"

State::State(): _parent(NULL), _name("nil") {}
State::~State() {}

void State::notifyRegistered(ParentState *parent, const std::string &name) {
    _parent = parent;
    _name = name;
}

void State::keyTyped(KeyEvent *event) { }
void State::keyPressed(KeyEvent *event) { }
void State::keyReleased(KeyEvent *event) { }

void State::mouseClicked(MouseButtonEvent *event) { }
void State::mousePressed(MouseButtonEvent *event) { }
void State::mouseReleased(MouseButtonEvent *event) { }
void State::mouseMoved(MouseMotionEvent *event) { }
