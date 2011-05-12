/*
 *  MHCore.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include <Base/FileSystem.h>
#include <Base/Exception.h>

#include <Content/ResourceGroupManager.h>
#include <Content/BasicMaterial.h>
#include <Content/MaterialManager.h>
#include <Content/ModelManager.h>
#include <Content/Content.h>

#include <Engine/AudioSystem.h>
#include <Engine/Keyboard.h>

#include "RenderContextBindings.h"
#include "OptionsModuleBindings.h"
#include "WindowBindings.h"

#include "MHCore.h"
//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHCore::MHCore(): DefaultCore(
    "Mountainhome"
#if DEBUG
#ifdef DEBUG_RESOURCE_DIR
    , DEBUG_RESOURCE_DIR
#else
#error DEBUG_RESOURCE_DIR is not defined.
#endif // ifdef DEBUG_RESOURCE_DIR
#else
#error Should be in debug mode.
#endif // if DEBUG
) {
    // Set the name of the state.
    _name = "Mountainhome";

    // Register some default resources.
    ///\todo XXXBMW: This REALLY must be doable from elsewhere.
    Texture::DefaultMinFilter = GL_LINEAR_MIPMAP_LINEAR;
    Texture::DefaultMagFilter = GL_LINEAR;

    // Make our two main textures nice and pretty.
    ///\todo XXXBMW: This REALLY must be doable from elsewhere.
    Content::GetOrLoad<Texture>("cursor-black.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Content::GetOrLoad<Texture>("cursor-black.png")->setTexCoordHandling(GL_CLAMP, GL_CLAMP);

    Material *white = Content::GetOrLoad<Material>("white");
//    Material *dwarfMat = new BasicMaterial(Vector4(0.6, 0.6, 0.6, 1.0), Vector4(0.8, 0.8, 0.8, 1.0));
//    Content::GetMaterialManager()->registerResource("Dwarf", dwarfMat);

    // Rotate loaded models on the x-axis to change their up from y to z.
    Content::GetModelManager()->setDefaultTransform(SQT(Quaternion::FromEuler(Degree(45), Radian(0), Radian(0)), Vector3(0, 0, 0)));

    Content::GetModelManager()->registerResource("Reticle", Model::CreateRectangle("Quad", white, Vector2(1.0, 1.0)));
    Content::GetModelManager()->registerResource("Dwarf", Content::GetOrLoad<Model>("dwarf_miner_2.fbx"));
    Content::GetModelManager()->registerResource("Tree", Content::GetOrLoad<Model>("plant_young.fbx"));

    // Rotate gear model to change its up from... something? to z.
    Content::GetModelManager()->setDefaultTransform(SQT(Quaternion::FromEuler(Degree(135), Radian(0), Radian(0)), Vector3(0, 0, 0)));
    Content::GetModelManager()->registerResource("Boulder", Content::GetOrLoad<Model>("gear_cluster.fbx"));

    // Temporary code!!!
    // TODO: The Content system should have something to do with audio files.
    // _audioSystem->playMusic("./UI/intro.ogg");
}

MHCore::~MHCore() {}

void MHCore::innerLoop(int elapsed) {
    DefaultCore::innerLoop(elapsed);

    char buffer[64];
    snprintf(buffer, 64, "FPS: %i Renderables: %i Primitives: %i Vertices: %i",
        (int)_framerate,
        _renderContext->getRenderableCount(),
        _renderContext->getPrimitiveCount(),
        _renderContext->getVertexCount());

    _mainWindow->setPostCaption(buffer);
}

void MHCore::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_F1:
        stopMainLoop();
        break;
	default:
        ParentState::keyPressed(event);
    }
}
