/*
 *  MHConceptState.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include <Render/Viewport.h>
#include <Render/RenderTarget.h>
#include <Render/SceneManager.h>
#include <Render/Material.h>
#include <Render/Camera.h>
#include <Render/Entity.h>
#include <Render/Light.h>
#include <Render/Node.h>

#include <Render/Sphere.h>

#include "TextureLoaderSDL.h"

#include <Engine/Window.h>
#include <Engine/Keyboard.h>

#include "MHCore.h"
#include "MHConceptState.h"

const float MHConceptState::Speed = 0.02;

MHConceptState::MHConceptState(): _gameScene(NULL), _sphere(NULL),
_r(NULL), _g(NULL), _b(NULL), _delta(-Speed), _moveLight(true) {
    // Create some basic stuff.
    _gameScene = new SceneManager();
    _sphere    = new Sphere(4);

    // Some very basic materials to work with.
    _r = new Material();
	_r->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    _r->setAmbient(1.0f, 1.0f, 1.0f);
    _r->setDiffuse(1.0, 0.0, 0.0, 1.0);

    _g = new Material();
	_g->setColor(0.0f, 1.0f, 0.0f, 1.0f);
    _g->setAmbient(1.0f, 1.0f, 1.0f);
    _g->setDiffuse(0.0, 1.0, 0.0, 0.75);

    _b = new Material();
	_b->setColor(0.0f, 0.0f, 1.0f, 1.0f);
    _b->setAmbient(1.0f, 1.0f, 1.0f);
    _b->setDiffuse(0.0, 0.0, 1.0, 1.0);
}

MHConceptState::~MHConceptState() {}

void MHConceptState::setup(va_list args) {
	/*
	// Load a texture
	Texture *rText = TextureManager::Get()->loadResource("font.png");
	*/

    // Setup the cameras.
    _lCam = _gameScene->createCamera("leftCamera");
    _lCam->setPosition(Vector3(0, 0, 0));
    _lCam->lookAt(Vector3(-10, 0, -10));

    _rCam = _gameScene->createCamera("rightCamera");
    _rCam->setPosition(Vector3(10, 0, 0));
    _rCam->lookAt(Vector3(10, 0, -10));

    _activeCam = _lCam;

    // Connect the left sphere to a child node.
    Entity *lSphere = _gameScene->createEntity(_sphere, "leftSphere");
    lSphere->setMaterial(_r);

    Node *lNode = _gameScene->getRootNode()->createChildNode("leftNode");
    lNode->setPosition(-10.0f, 0.0f, -10.0f);
    lNode->attach(lSphere);

    // Connect the right sphere directly to the root scene node
    Entity *rSphere = _gameScene->createEntity(_sphere, "rightSphere");
    rSphere->setPosition(10.0f, 0.0f, -10.0f);
    rSphere->setMaterial(_g);

    _gameScene->getRootNode()->attach(rSphere);

    // And let there be light!!!
    Light *l = _gameScene->createLight("mainLight");
    l->setPosition(0.0f, 0.0f, 0.0f);
    l->setAmbient(0.2f, 0.2f, 0.2f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);

    // Connect our cameras to the window.
    MHCore::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
    MHCore::GetWindow()->addViewport(_gameScene->getCamera("leftCamera" ), 0, 0.0f, 0.0f, 0.5f, 1.0f);
    MHCore::GetWindow()->addViewport(_gameScene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

void MHConceptState::teardown() {
    _gameScene->clearScene();
}

void MHConceptState::update(int elapsed) {
    Light *l = _gameScene->getLight("mainLight");
    if (l->getPosition().y >  10.0f) { _delta = -Speed; }
    if (l->getPosition().y < -10.0f) { _delta =  Speed; }

    // Move the light.
    if (_moveLight) {
        l->setPosition(l->getPosition() + (Vector3(0.0f, _delta * elapsed, 0.0f)));
    }

    _activeCam->moveRelative(_move * elapsed);
    _activeCam->rotate(Quaternion(_yaw * elapsed, _pitch * elapsed, Radian(0)));
    _yaw = _pitch = 0; // Clear the rotation data so we don't spin forever.
}

void MHConceptState::keyPressed(KeyEvent *event) {
    static Real moveSpeed = 0.01;

    switch(event->key()) {
    case Keyboard::KEY_UP:    _move.z = -moveSpeed; break;
    case Keyboard::KEY_DOWN:  _move.z =  moveSpeed; break;
    case Keyboard::KEY_LEFT:  _move.x = -moveSpeed; break;
    case Keyboard::KEY_RIGHT: _move.x =  moveSpeed; break;
    case Keyboard::KEY_RETURN:    
        _moveLight = !_moveLight;
        break;
    case Keyboard::KEY_SPACE:
        _activeCam = (_activeCam == _lCam) ? _rCam : _lCam;
        break;
    }
}

void MHConceptState::keyReleased(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_UP:    _move.z = 0; break;
    case Keyboard::KEY_DOWN:  _move.z = 0; break;
    case Keyboard::KEY_LEFT:  _move.x = 0; break;
    case Keyboard::KEY_RIGHT: _move.x = 0; break;
    }
}

void MHConceptState::mouseMoved(MouseMotionEvent *event) {
    static Real rotSpeed = 0.01;
    _yaw   = event->relX() * rotSpeed;
    _pitch = event->relY() * rotSpeed;
}

