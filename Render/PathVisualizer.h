#ifndef _PATHVISUALIZER_H_
#define _PATHVISUALIZER_H_

#include "Renderable.h"

class PathVisualizer : public Renderable {
    public:
        PathVisualizer(std::vector<Edge> edgeVec) {
            _edges = edgeVec;
        }
        ~PathVisualizer() {}

        void render(RenderContext *context) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

            glBegin(GL_LINES);
            for(int c = 0; c < _edges.size(); c++) {
                glVertex3fv(_edges[c].first.array);
                glVertex3fv(_edges[c].second.array);
            }
            glEnd();

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

    private:
        std::vector<Edge> _edges;
};

#endif
