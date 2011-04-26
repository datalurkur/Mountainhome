#ifndef _PATHVISUALIZER_H_
#define _PATHVISUALIZER_H_

#include <Engine/Entity.h>
#include "PathManager.h"

class PathVisualizer : public Entity {
    public:
        PathVisualizer(PathManager *pathManager);
        ~PathVisualizer();

        void update();

    private:
        PathManager *_pathManager;
};

#endif
