#ifndef _MHCAMERA_H_
#define _MHCAMERA_H_

#include "RubyBindings.h"
#include <Render/Camera.h>

class MHCamera: public Camera, public ManyObjectBinding<MHCamera> {
public:
    MHCamera(SceneManager *scene, int level);
    virtual ~MHCamera();

    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE world, VALUE level);
	static VALUE SetFixedYaw(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE SetPosition(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE LookAt(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE RotateOnAxis(VALUE self, VALUE amt, VALUE x, VALUE y, VALUE z);
private:
};

#endif
