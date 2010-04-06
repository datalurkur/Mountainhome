#include "MHTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHTerrain::SetupBindings() {
    Class = rb_define_class("MHTerrain", rb_cObject);
    rb_define_method(Class, "set_tile", RUBY_METHOD_FUNC(MHTerrain::SetTile), 4);
    rb_define_method(Class, "get_tile", RUBY_METHOD_FUNC(MHTerrain::GetTile), 3);
    rb_define_method(Class, "get_surface", RUBY_METHOD_FUNC(MHTerrain::SurfaceTile), 2);
    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHTerrain::GetWidth), 0);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHTerrain::GetHeight), 0);
    rb_define_method(Class, "depth", RUBY_METHOD_FUNC(MHTerrain::GetDepth), 0);
}

VALUE MHTerrain::GetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}

VALUE MHTerrain::SetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE type) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    cSelf->setTile(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(type));
    return rSelf;
}

VALUE MHTerrain::SurfaceTile(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE MHTerrain::GetWidth(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->_rootGroup->getDims()[0]);
}

VALUE MHTerrain::GetHeight(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->_rootGroup->getDims()[1]);
}

VALUE MHTerrain::GetDepth(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->_rootGroup->getDims()[2]);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain definitions
//////////////////////////////////////////////////////////////////////////////////////////
MHTerrain::MHTerrain(int width, int height, int depth) {
    _rootGroup = new TileGroup<short>(Vector3(0, 0, 0), Vector3(width, height, depth), 0, 0);
}

MHTerrain::~MHTerrain() {}

short MHTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(Vector3(x, y, z));
}

void MHTerrain::setTile(int x, int y, int z, short type) {
    _rootGroup->setTile(Vector3(x, y, z), type);
}

int MHTerrain::getSurfaceLevel(int x, int y) {
    int nX = x,
        nY = y;
    Vector3 dims = _rootGroup->getDims();
    
    if(nX < 0) { nX = 0; }
    else if(nX >= dims[0]) { nX = dims[0]-1; }
    if(nY < 0) { nY = 0; }
    else if(nY >= dims[1]) { nY = dims[1]-1; }
    
    return _rootGroup->getSurfaceLevel(Vector2(nX, nY));
}

