#include "MHPath.h"
#include "MHWorld.h"

// Ruby bindings and binding setup
void MHPath::SetupBindings() {
    Class = rb_define_class("MHPath", rb_cObject);
	
    rb_define_method(Class, "next_step", RUBY_METHOD_FUNC(MHPath::NextStep), 0);
}

VALUE MHPath::NextStep(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    Vector3 cStep = cSelf->getNextStep();

    VALUE *coords = new VALUE[3];
    coords[0] = INT2NUM(cStep[0]);
    coords[1] = INT2NUM(cStep[1]);
    coords[2] = INT2NUM(cStep[2]);

    return rb_ary_new4(3, coords);
}

// C-functions
MHPath::MHPath(Vector3 source, Vector3 dest, MHWorld *world) {
    findPath(source, dest, &path, world->getTerrain());
}

bool MHPath::endOfPath() {
    return path.empty();
}

Vector3 MHPath::getNextStep() {
    Vector3 nStep = path.top();
    path.pop();
    return nStep;
}
