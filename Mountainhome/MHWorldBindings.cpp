/*
 *  MHWorldBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHWorldBindings.h"
#include "CameraBindings.h"
#include "MHCoreBindings.h"
#include "MHTerrainBindings.h"
#include "EntityBindings.h"

#include "MHActorBindings.h"
#include "PathManagerBindings.h"
#include "MHSelectionBindings.h"
#include "RenderContextBindings.h"

MHWorldBindings::MHWorldBindings()
: RubyBindings<MHWorld, true>(
    rb_define_class("MHWorld", rb_cObject),
    "MHWorldBindings")
{
    rb_define_method(_class, "initialize", RUBY_METHOD_FUNC(MHWorldBindings::Initialize), 1);

    rb_define_method(_class, "active_camera=", RUBY_METHOD_FUNC(MHWorldBindings::SetActiveCamera), 1);
    rb_define_method(_class, "active_camera", RUBY_METHOD_FUNC(MHWorldBindings::GetActiveCamera), 0);
    rb_define_method(_class, "frustum_culling=", RUBY_METHOD_FUNC(MHWorldBindings::SetFrustumCulling), 1);
    rb_define_method(_class, "draw_bounding_boxes=", RUBY_METHOD_FUNC(MHWorldBindings::SetDrawBoundingBoxes), 1);
    rb_define_method(_class, "draw_path_visualizer=", RUBY_METHOD_FUNC(MHWorldBindings::SetPathVisualizer), 1);
    rb_define_method(_class, "render", RUBY_METHOD_FUNC(MHWorldBindings::Render), 1);

    rb_define_method(_class, "terrain", RUBY_METHOD_FUNC(MHWorldBindings::GetTerrain), 0);
    rb_define_method(_class, "pathfinder", RUBY_METHOD_FUNC(MHWorldBindings::GetPathFinder), 0);

    rb_define_method(_class, "populate", RUBY_METHOD_FUNC(MHWorldBindings::Populate), 0);

    rb_define_method(_class, "create_actor", RUBY_METHOD_FUNC(MHWorldBindings::CreateActor), 4);
    rb_define_method(_class, "create_entity", RUBY_METHOD_FUNC(MHWorldBindings::CreateEntity), 4);
    rb_define_method(_class, "create_camera", RUBY_METHOD_FUNC(MHWorldBindings::CreateCamera), -1);

    rb_define_method(_class, "selection", RUBY_METHOD_FUNC(MHWorldBindings::GetSelection), 0);

    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHWorldBindings::GetWidth), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHWorldBindings::GetHeight), 0);
    rb_define_method(_class, "depth", RUBY_METHOD_FUNC(MHWorldBindings::GetDepth), 0);

    rb_define_method(_class, "save", RUBY_METHOD_FUNC(MHWorldBindings::Save), 1);
    rb_define_method(_class, "load", RUBY_METHOD_FUNC(MHWorldBindings::Load), 1);
    rb_define_method(_class, "load_empty", RUBY_METHOD_FUNC(MHWorldBindings::LoadEmpty), 4);

    rb_define_method(_class, "pick_objects", RUBY_METHOD_FUNC(MHWorldBindings::PickObjects), 5);

    rb_define_alloc_func(_class, MHWorldBindings::Alloc<MHWorldBindings>);
}

void MHWorldBindings::Mark(MHWorld* world) {
    rb_gc_mark(MHTerrainBindings::Get()->getValue(world->getTerrain()));
    rb_gc_mark(PathManagerBindings::Get()->getValue(world->getPathFinder()));
    rb_gc_mark(MHSelectionBindings::Get()->getValue(world->getSelection()));
}

VALUE MHWorldBindings::Initialize(VALUE rSelf, VALUE rCore) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    MHCore *cCore = MHCoreBindings::Get()->getPointer(rCore);
    cSelf->initialize(cCore);

    NEW_RUBY_OBJECT(MHSelectionBindings, cSelf->getSelection());

    return rSelf;
}

VALUE MHWorldBindings::GetActiveCamera(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return CameraBindings::Get()->getValue(cSelf->getActiveCamera());
}

VALUE MHWorldBindings::SetActiveCamera(VALUE rSelf, VALUE rCam) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    Camera *cCam = CameraBindings::Get()->getPointer(rCam);
    cSelf->setActiveCamera(cCam);
    return Qnil;
}

VALUE MHWorldBindings::SetFrustumCulling(VALUE rSelf, VALUE rVal) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    cSelf->getScene()->setFrustumCulling((rVal != Qfalse && rVal != Qnil) ? true : false);
    return rVal;
}

VALUE MHWorldBindings::SetDrawBoundingBoxes(VALUE rSelf, VALUE rVal) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    cSelf->getScene()->setDrawBoundingBoxes((rVal != Qfalse && rVal != Qnil) ? true : false);
    return rVal;
}

VALUE MHWorldBindings::SetPathVisualizer(VALUE rSelf, VALUE rVal) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    if(rVal == Qtrue) { cSelf->showPath(); }
    else { cSelf->hidePath(); }
    return rVal;
}

VALUE MHWorldBindings::Render(VALUE rSelf, VALUE rContext) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    RenderContext *cContext = RenderContextBindings::Get()->getPointer(rContext);
    cSelf->render(cContext);    
    return Qnil;
}

VALUE MHWorldBindings::CreateEntity(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material) {
    return Create<Entity, EntityBindings>(rSelf, klass, name, model, material);
}

VALUE MHWorldBindings::CreateActor(VALUE rSelf, VALUE klass, VALUE name, VALUE model, VALUE material) {
    return Create<MHActor, MHActorBindings>(rSelf, klass, name, model, material);
}

VALUE MHWorldBindings::CreateCamera(int argc, VALUE *argv, VALUE rSelf) {
    ASSERT(argc >= 2);

    VALUE rCameraName = argv[0];
    VALUE klass = argv[1];

    // Replace the class argument with the world reference for the default Camera c'tor.
    argv[1] = rSelf;

    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    std::string cCameraName = rb_string_value_cstr(&rCameraName);
    Camera *cam = cSelf->createCamera(cCameraName);

    VALUE rCam = NEW_RUBY_OBJECT_FULL(CameraBindings, cam, klass);
    rb_obj_call_init(rCam, argc, argv);

    return rCam;
}

VALUE MHWorldBindings::Populate(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    cSelf->populate();
    return rSelf;
}

VALUE MHWorldBindings::GetTerrain(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return MHTerrainBindings::Get()->getValue(cSelf->getTerrain());
}

VALUE MHWorldBindings::GetPathFinder(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return PathManagerBindings::Get()->getValue(cSelf->getPathFinder());
}

VALUE MHWorldBindings::GetSelection(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return MHSelectionBindings::Get()->getValue(cSelf->getSelection());
}

VALUE MHWorldBindings::GetWidth(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHWorldBindings::GetHeight(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHWorldBindings::GetDepth(VALUE rSelf) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getDepth());
}

VALUE MHWorldBindings::Save(VALUE rSelf, VALUE world) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    std::string cWorld = rb_string_value_cstr(&world);

    cSelf->save(cWorld);

    return rSelf;
}

VALUE MHWorldBindings::Load(VALUE rSelf, VALUE world) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    std::string cWorld = rb_string_value_cstr(&world);
    cSelf->load(cWorld);
    NEW_RUBY_OBJECT(PathManagerBindings, cSelf->getPathFinder());
    NEW_RUBY_OBJECT(MHTerrainBindings, cSelf->getTerrain());
    return rSelf;
}

VALUE MHWorldBindings::LoadEmpty(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    MHCore *cCore = MHCoreBindings::Get()->getPointer(rCore);
    cSelf->loadEmpty(NUM2INT(width), NUM2INT(height), NUM2INT(depth), cCore);
    NEW_RUBY_OBJECT(PathManagerBindings, cSelf->getPathFinder());
    NEW_RUBY_OBJECT(MHTerrainBindings, cSelf->getTerrain());
    return rSelf;
}

VALUE MHWorldBindings::PickObjects(VALUE rSelf, VALUE rCam, VALUE startX, VALUE startY, VALUE endX, VALUE endY) {
    MHWorld *cSelf = MHWorldBindings::Get()->getPointer(rSelf);
    Camera *cCam = CameraBindings::Get()->getPointer(rCam);

    cSelf->pickObjects(cCam, NUM2DBL(startX), NUM2DBL(startY), NUM2DBL(endX), NUM2DBL(endY));
    return GetSelection(rSelf);
}
