#include "MHCamera.h"
#include "MHWorld.h"
#include "MHcore.h"

#include <Engine/Window.h>

MHCamera::MHCamera(SceneManager *scene, int level): Camera(scene) {
	MHCore::GetWindow()->addRenderSource((Camera*)this, level);
}

MHCamera::~MHCamera() {
}

void MHCamera::SetupBindings() {
    Class = rb_define_class("MHCamera", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHCamera::Initialize), 2);
	rb_define_method(Class, "set_fixed_yaw", RUBY_METHOD_FUNC(MHCamera::SetFixedYaw), 3);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(MHCamera::SetPosition), 3);
	rb_define_method(Class, "look_at", RUBY_METHOD_FUNC(MHCamera::LookAt), 3);
	rb_define_method(Class, "rotate_on_axis", RUBY_METHOD_FUNC(MHCamera::RotateOnAxis), 4);
	rb_define_method(Class, "move_relative", RUBY_METHOD_FUNC(MHCamera::MoveRelative), 3);
}

VALUE MHCamera::Initialize(VALUE self, VALUE world, VALUE level) {
	MHWorld *pWorld = (MHWorld*)MHWorld::GetObject(world);
	int iLevel = NUM2INT(level);
	
    MHCamera::RegisterObject(self, new MHCamera((SceneManager*)pWorld->getScene(), iLevel));
    return self;
}

VALUE MHCamera::SetFixedYaw(VALUE self, VALUE x, VALUE y, VALUE z) {
	MHCamera *camera = (MHCamera*)GetObject(self);
	int iX = NUM2INT(x),
	    iY = NUM2INT(y),
	    iZ = NUM2INT(z);
	camera->setFixedYawAxis(true, Vector3(iX, iY, iZ));
	
	return self;
}

VALUE MHCamera::SetPosition(VALUE self, VALUE x, VALUE y, VALUE z) {
	MHCamera *camera = (MHCamera*)GetObject(self);
	float fX = NUM2DBL(x),
		  fY = NUM2DBL(y),
		  fZ = NUM2DBL(z);
	camera->setPosition(Vector3(fX, fY, fZ));
	
	return self;
}

VALUE MHCamera::LookAt(VALUE self, VALUE x, VALUE y, VALUE z) {
	MHCamera *camera = (MHCamera*)GetObject(self);
	float fX = NUM2DBL(x),
		  fY = NUM2DBL(y),
		  fZ = NUM2DBL(z);
	camera->lookAt(Vector3(fX, fY, fZ));
	
	return self;
}

VALUE MHCamera::RotateOnAxis(VALUE self, VALUE amt, VALUE x, VALUE y, VALUE z) {
	MHCamera *camera = (MHCamera*)GetObject(self);
	float fAmt = NUM2DBL(amt);
	float fX = NUM2DBL(x),
		  fY = NUM2DBL(y),
		  fZ = NUM2DBL(z);
		
	if(fX > 0.0) {
		camera->adjustPitch(Radian(fAmt * fX));
	}
	
	if(fY > 0.0) {
		camera->adjustRoll(Radian(fAmt * fY));
	}
	
	if(fZ > 0.0) {
		camera->adjustYaw(Radian(fAmt * fZ));
	}
	
	return self;
}

VALUE MHCamera::MoveRelative(VALUE self, VALUE x, VALUE y, VALUE z) {
	MHCamera *camera = (MHCamera*)GetObject(self);
	float fX = NUM2DBL(x),
		  fY = NUM2DBL(y),
		  fZ = NUM2DBL(z);
		  
	camera->moveRelative(Vector3(fX, fY, fZ));
	
	return self;
}
