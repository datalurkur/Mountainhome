/*
 *  GameState.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include <Render/Viewport.h>
#include <Render/RenderTarget.h>
#include <Render/OctreeScene.h>
#include <Render/Material.h>
#include <Render/Camera.h>
#include <Render/Entity.h>
#include <Render/Light.h>
#include <Render/Node.h>

#include <Render/Sphere.h>

#include "TextureLoaderSDL.h"

#include <Engine/Window.h>
#include <Engine/Keyboard.h>

#include "Mountainhome.h"
#include "GameState.h"

const float GameState::Speed = 0.02;

GameState::GameState(): _gameScene(NULL), _sphere(NULL),
_r(NULL), _g(NULL), _b(NULL), _delta(-Speed), _move(true) {
    // Create some basic stuff.
    _gameScene = new OctreeScene();
    _sphere    = new Sphere(4);

    // Some very basic materials to work with.
    _r = new Material("red");
	_r->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    _r->setAmbient(1.0f, 1.0f, 1.0f);
    _r->setDiffuse(1.0, 0.0, 0.0, 1.0);

    _g = new Material("green");
	_g->setColor(0.0f, 1.0f, 0.0f, 1.0f);
    _g->setAmbient(1.0f, 1.0f, 1.0f);
    _g->setDiffuse(0.0, 1.0, 0.0, 0.75);

    _b = new Material("blue");
	_b->setColor(0.0f, 0.0f, 1.0f, 1.0f);
    _b->setAmbient(1.0f, 1.0f, 1.0f);
    _b->setDiffuse(0.0, 0.0, 1.0, 1.0);
}

GameState::~GameState() {}

void GameState::setup(va_list args) {
	/*
	// Load a texture
	Texture *rText = TextureManager::Get()->loadResource("font.png");
	*/

    // Setup the cameras.
    Camera *lCam = _gameScene->createCamera("leftCamera");
    lCam->setPosition(Vector3(0, 0, 0));
    lCam->lookAt(Vector3(-10, 0, -10));

    Camera *rCam = _gameScene->createCamera("rightCamera");
    rCam->setPosition(Vector3(10, 0, 0));
    rCam->lookAt(Vector3(10, 0, -10));

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
    Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
    Mountainhome::GetWindow()->addViewport(_gameScene->getCamera("leftCamera" ), 0, 0.0f, 0.0f, 0.5f, 1.0f);
    Mountainhome::GetWindow()->addViewport(_gameScene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

void GameState::teardown() {
    Mountainhome::GetWindow()->removeAllViewports();
    _gameScene->clearScene();
}

void GameState::update(int elapsed) {
    Light *l = _gameScene->getLight("mainLight");
    if (l->getPosition().y >  10.0f) { _delta = -Speed; }
    if (l->getPosition().y < -10.0f) { _delta =  Speed; }

    // Move the light.
    if (_move) {
        l->setPosition(l->getPosition() + (Vector3(0.0f, _delta * elapsed, 0.0f)));
    }
}

void GameState::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_SPACE:
        _move = !_move;
        break;
    }
}
