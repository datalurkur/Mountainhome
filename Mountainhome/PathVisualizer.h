#ifndef _PATHVISUALIZER_H_
#define _PATHVISUALIZER_H_

#include <Engine/Entity.h>
#include "PathManager.h"

class PathVisualizer : public Entity {
    public:
        PathVisualizer(PathManager *pathManager);
        ~PathVisualizer();

        void update(bool drawNodes, bool drawEdges);
        void addPath(const std::vector<Vector3> &path);

        void clear();

    private:
        PathManager *_pathManager;
};

#endif
