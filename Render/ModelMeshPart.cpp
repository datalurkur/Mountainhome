#include "ModelMeshPart.h"

ModelMeshPart::ModelMeshPart(): _mat(0), _numIndices(0), _startIndex(0) {}

ModelMeshPart::ModelMeshPart(unsigned int numIndices, unsigned int startIndex):
    _mat(0), _numIndices(numIndices), _startIndex(startIndex) {}

ModelMeshPart::~ModelMeshPart() {}

unsigned int ModelMeshPart::getIndexCount() { return _numIndices; }
unsigned int ModelMeshPart::getStartIndex() { return _startIndex; }
