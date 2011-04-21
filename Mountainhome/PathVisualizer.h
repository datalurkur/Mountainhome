#ifndef _PATHVISUALIZER_H_
#define _PATHVISUALIZER_H_

#include <Engine/Entity.h>
#include "MHPathFinder.h"

class PathVisualizer : public Entity {
    public:
        PathVisualizer(int width, int height, int depth);
        ~PathVisualizer();

        void updateEdges(std::vector<Edge> &edges);
};

#endif
