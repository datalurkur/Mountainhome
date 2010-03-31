#include "MHTerrain.h"

MHTerrain::MHTerrain(int w, int h, int d) {
    _rootGroup = new TileGroup<short>(Vector3(0,0,0), Vector3(w,h,d), 0, 0);
}

MHTerrain::~MHTerrain() {
    MHTerrain::UnregisterObject(this);
}

void MHTerrain::SetupBindings() {
    Class = rb_define_class("MHTerrain", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHTerrain::Initialize), 3);
    rb_define_method(Class, "set_tile", RUBY_METHOD_FUNC(MHTerrain::SetTile), 4);
    rb_define_method(Class, "get_tile", RUBY_METHOD_FUNC(MHTerrain::GetTile), 3);
}

VALUE MHTerrain::Initialize(VALUE self, VALUE w, VALUE h, VALUE d) {
    int iW = NUM2INT(w),
        iH = NUM2INT(h),
        iD = NUM2INT(d);
    MHTerrain::RegisterObject(self, new MHTerrain(iW, iH, iD));
    return self;
}

VALUE MHTerrain::GetTile(VALUE self, VALUE x, VALUE y, VALUE z) {
    int iX = NUM2INT(x),
        iY = NUM2INT(y),
        iZ = NUM2INT(z);
    MHTerrain* terrain = (MHTerrain*)GetObject(self);

    return INT2NUM(terrain->getTile(iX, iY, iZ));
}

VALUE MHTerrain::SetTile(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type) {
    int iX    = NUM2INT(x),
        iY    = NUM2INT(y),
        iZ    = NUM2INT(z),
        iType = NUM2INT(type);
    MHTerrain* terrain = (MHTerrain*)GetObject(self);
    
    terrain->setTile(iX, iY, iZ, iType);

    return self;
}

short MHTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(Vector3(x, y, z));
}

void MHTerrain::setTile(int x, int y, int z, short type) {
    _rootGroup->setTile(Vector3(x, y, z), type);
}

int MHTerrain::getSurfaceLevel(int x, int y) {
    int surface = _rootGroup->getSurfaceLevel(Vector2(x, y));
    //Info("Found surface for " << x << "," << y << " at level " << surface);
    return surface;
}

