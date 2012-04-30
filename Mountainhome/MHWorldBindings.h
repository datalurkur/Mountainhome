/*
 *  MHWorldBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHWORLDBINDINGS_H_
#define _MHWORLDBINDINGS_H_
#include "RubyBindings.h"
#include "MHWorld.h"

#include "OctreeSceneManager.h"

#include <Content/Content.h>

// XXXBMW: There is a ruby bug that causes the world to never GC because we load it up
// with fibers. This means we have to destroy it manually.
class MHWorldBindings : public RubyBindings<MHWorld, true> {
public:
    /*! Creates the MHWorld bindings. */
    static void SetupBindings();

    /*! Marks the camera and world objects to keep the from being deleted. */
    static void Mark(MHWorld* world);

    /*! Creates a new instance of the MHWorld object and registers it as being associated
     *  with the given ruby object VALUE.
     * \param self The ruby space World object.
     * \param width The width of the world in voxels.
     * \param height The height of the world in voxels.
     * \param depth The depth of the world in voxels. */
    static VALUE Initialize(VALUE self, VALUE rCore);

    /*! Gets the currently active camera. */
    static VALUE GetActiveCamera(VALUE self);

    /*! Sets the currently active camera. */
    static VALUE SetActiveCamera(VALUE self, VALUE camera);

    /*! Used to toggle frustum culling. */
    static VALUE SetFrustumCulling(VALUE self, VALUE value);

    /*! Used to toggle bounding boxes. */
    static VALUE SetDrawBoundingBoxes(VALUE self, VALUE value);

    /*! Used to toggle the path visualizer */
    static VALUE SetPathVisualizer(VALUE self, VALUE value);

    /*! Renders the current scene with the given RenderContext. */
    static VALUE Render(VALUE self, VALUE context);

    /*! Calls on scenemanager to create a new camera object. Takes a camera name, followed
     *  by a klass type to create, followed optionally by any other arguments needed to
     *  create a camera of the given type. */
    static VALUE CreateCamera(int argc, VALUE *argv, VALUE rSelf);

    /*! Terrain getter. */
    static VALUE GetTerrain(VALUE self);

    /*! PathFinder getter. */
    static VALUE GetPathFinder(VALUE self);

    /*! LiquidManager getter. */
    static VALUE GetLiquidManager(VALUE self);

    /*! Tells the world to generate geometry in the scene.
     * \param self The ruby space World object. */
    static VALUE Populate(VALUE self);

    /*! Defers to Create<T1, T2>. Need to do this because of issues with pointers to
     *  function templates. */
    static VALUE CreateEntity(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material);

    /*! Defers to Create<T1, T2>. Need to do this because of issues with pointers to
     *  function templates. */
    static VALUE CreateActor(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material);

    static VALUE DestroyEntity(VALUE rSelf, VALUE name);
    static VALUE DestroyActor(VALUE rSelf, VALUE name);

    /*! Scene object creation. Can be used to create derivatives of T. */
    template <typename T, typename TBindings>
    static VALUE Create(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material);

    template <typename T, typename TBindings>
    static VALUE Destroy(VALUE rSelf, VALUE name);

	/*! Get the selection in this world. */
	static VALUE GetSelection(VALUE self);
	static VALUE GetActorSelection(VALUE self);
	static VALUE GetVoxelSelection(VALUE self);
    
    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

    /*! Import/Export the world. */
    static VALUE Save(VALUE self, VALUE world);
    static VALUE Load(VALUE self, VALUE world);
    static VALUE LoadEmpty(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore);

    /*! Do picking. */
    static VALUE PickObjects(VALUE rSelf, VALUE rCam, VALUE rLeft, VALUE rBottom, VALUE rRight, VALUE rTop);

    /*! Work around for fiber bug in ruby. Deletes the World object. */
    static VALUE DestroyInstance(VALUE rSelf);

public:
    MHWorldBindings();
    virtual ~MHWorldBindings() {}

private:
    ID _terrain;
};

template <typename T, typename TBindings>
VALUE MHWorldBindings::Create(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);

    // Setup the object.
    T* cEntity = new T(rb_string_value_cstr(&name));
    cSelf->getScene()->addNode(cEntity);

    // Handle the model, if there is one.
    if (model != Qnil) {
        Material *cMaterial = NULL;
        if (material != Qnil) {
            std::string cMaterialName = rb_string_value_cstr(&material);
            cMaterial = Content::GetOrLoad<Material>(cMaterialName);
        }

        std::string cModelName = rb_string_value_cstr(&model);
        Model *cModel = Content::GetOrLoad<Model>(cModelName);

        cEntity->addModel(cModel, cMaterial);
    }

    // define and return new Ruby-side Actor-or-MHEntity class object
    VALUE obj = NEW_RUBY_OBJECT_FULL(TBindings, cEntity, klass);
    // call MountainhomeObjectModule's initialize function with no arguments
    VALUE args = rb_ary_new();
    rb_obj_call_init(obj, 0, &args);
    return obj;
}

template <typename T, typename TBindings>
VALUE MHWorldBindings::Destroy(VALUE rSelf, VALUE name) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
//    T *cEntity = TBindings::Get()->getPointer(rSelf);

    const std::string str(rb_string_value_cstr(&name));
    cSelf->getScene()->deleteNode<T>(str);

    return Qnil;
}

#endif
