#include "Path.h"
#include "MHWorld.h"

Path::Path(Vector3 source, Vector3 dest, MHWorld *world) {
    findPath(source, dest, &path, world->getTerrain());
}

bool Path::endOfPath() {
    return path.empty();
}

Vector3 Path::getNextStep() {
    Vector3 nStep = path.top();
    path.pop();
    return nStep;
}
