#include "MHPath.h"
#include "MHWorld.h"

#pragma mark Ruby Bindings
void MHPath::SetupBindings() {
    Class = rb_define_class("MHPath", rb_cObject);
	
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHPath::Initialize), 7);
    rb_define_method(Class, "next_step", RUBY_METHOD_FUNC(MHPath::NextStep), 0);
    rb_define_method(Class, "blocked?", RUBY_METHOD_FUNC(MHPath::Blocked), 0);
    rb_define_method(Class, "end_of_path?", RUBY_METHOD_FUNC(MHPath::EndOfPath), 0);
    rb_define_alloc_func(Class, MHPath::Alloc);
}

VALUE MHPath::Initialize(VALUE rSelf, VALUE rWorld, VALUE rSX, VALUE rSY, VALUE rSZ, VALUE rDX, VALUE rDY, VALUE rDZ) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    AssignCObjFromValue(MHWorld, cWorld, rWorld);

    cSelf->initialize(cWorld, Vector3(NUM2INT(rSX), NUM2INT(rSY), NUM2INT(rSZ)),
                              Vector3(NUM2INT(rDX), NUM2INT(rDY), NUM2INT(rDZ)));
    return rSelf;
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

VALUE MHPath::Blocked(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    return (cSelf->blocked()? Qtrue : Qfalse);
}

VALUE MHPath::EndOfPath(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    return (cSelf->path.empty()? Qtrue : Qfalse);
}

#pragma mark C Functions

MHPath::MHPath() { }

MHPath::~MHPath() { }

void MHPath::initialize(MHWorld *world, Vector3 source, Vector3 dest) {
    accessible = findPath(source, dest, &path, world->getTerrain());
}

bool MHPath::blocked() {
    return !accessible;
}

bool MHPath::endOfPath() {
    return path.empty();
}

Vector3 MHPath::getNextStep() {
    Vector3 nStep = path.top();
    path.pop();
    return nStep;
}
