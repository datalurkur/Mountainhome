#include "ModelBone.h"

ModelBone::ModelBone(): _children(0), _parent(0) {
    _transform.loadIdentity();
}

ModelBone::ModelBone(const Matrix &transform, ModelBone *parent) {
    _transform = transform;
    _parent = parent;
}

ModelBone::~ModelBone() {}
