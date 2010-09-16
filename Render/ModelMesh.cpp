#include "ModelMesh.h"

ModelMesh::ModelMesh():
    _parts(0), _numParts(0), _materials(0) {}

ModelMesh::ModelMesh(ModelMeshPart *parts, unsigned int numParts):
    _parts(parts), _numParts(numParts), _materials(0) {}

ModelMesh::~ModelMesh() { }

ModelMeshPart* ModelMesh::getPart(int index) { return &_parts[index]; }

unsigned int ModelMesh::getPartCount() { return _numParts; }
