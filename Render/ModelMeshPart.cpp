#include "ModelMeshPart.h"

ModelMeshPart::ModelMeshPart(unsigned int *indices, unsigned int numIndices, unsigned int startIndex):
    _indices(indices), _numIndices(numIndices), _startIndex(startIndex) {}

ModelMeshPart::~ModelMeshPart() {}

unsigned int ModelMeshPart::getIndexCount() { return _numIndices; }
unsigned int ModelMeshPart::getStartIndex() { return _startIndex; }
