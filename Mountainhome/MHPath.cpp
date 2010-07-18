#include "MHPath.h"
#include "MHWorld.h"

// Ruby bindings and binding setup

#pragma mark Ruby Bindings
void MHPath::SetupBindings() {
    Class = rb_define_class("MHPath", rb_cObject);
	
    rb_define_method(Class, "next_step", RUBY_METHOD_FUNC(MHPath::NextStep), 0);
    rb_define_method(Class, "end_of_path?", RUBY_METHOD_FUNC(MHPath::EndOfPath), 0);
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

VALUE MHPath::EndOfPath(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    return (cSelf->path.empty()? Qtrue : Qfalse);
}

#pragma mark C Functions

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
