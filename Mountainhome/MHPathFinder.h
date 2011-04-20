/*
 * MHPathFinder.h
 * Mountainhome
 *
 * Created by Andrew Jean on 12/25/10
 * Copyright 2010 Mountainhome Project. All rights reserved.
 */

#ifndef _MHPATHFINDER_H_
#define _MHPATHFINDER_H_

#include <stack>
#include <vector>
#include <utility>
#include <Base/Vector.h>
#include <Boost/graph/graph_traits.hpp>
#include <Boost/graph/adjacency_list.hpp>
#include <Boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

// Get all these typedefs out of the way so as not to clutter up the code
typedef property<edge_weight_t, int> Weight;

// The templating choices here are *very* important, effecting search time, memory usable, and stability
typedef adjacency_list<listS, vecS, undirectedS, no_property, Weight> Graph;
/* listS is used as the first parameter to indicate what data structure should store the edges; listS allows us much greater
     efficiency in adding and removing edges than vecS; setS requires more memory, but allows us to not care about parallel
     (duplicate) edges in the graph; for now, listS is the best choice unless parallel edges become an issue
   vecS is used as the second parameter to indicate what data structure should store the vertices;
     since we don't need to add/remove vertices once the graph has been initialized, we use vector for its memory efficiency
   undirectedS indicates the graph is not a directional graph (edges can be traversed either direction)
     vertices don't have any properties (indicated by no_property), but edges have a weight, defined above
*/

typedef graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef graph_traits<Graph>::edge_descriptor EdgeDescriptor;
typedef std::vector<VertexDescriptor> PredecessorMap;
typedef std::vector<int> DistanceMap;

typedef std::pair<int, int> Neighbor;
typedef std::pair<Vector3, Vector3> Edge;

// These values are rounded to allow us to use integers.  If more precision is needed, 
//  they could be upgraded to floats without too much trouble
#define NORMAL_WEIGHT   10
#define DIAGONAL_WEIGHT 14
#define CORNER_WEIGHT   17

class MHPathFinder {
    public:
        MHPathFinder(int width, int height, int depth);
        ~MHPathFinder();

        void tileBlocked(int x, int y, int z);
        void tileUnblocked(int x, int y, int z);

		void zRangeBlocked(int x, int y, int start_z, int end_z);
		void zRangeUnblocked(int x, int y, int start_z, int end_z);

        void setStartPosition(int x, int y, int z);
        int getPathTo(int x, int y, int z, std::stack<Vector3> &path);
        bool isPathBlocked(int x, int y, int z);
        void getClosestPath(std::stack<Vector3> destinations, std::stack<Vector3> &path);
        void getFirstPath(std::stack<Vector3> destinations, std::stack<Vector3> &path);

        Graph *getGraph() const { return _graph; }
        Vector3 getIndexTile(int index) const;

        void getEdges(std::vector<Edge> &edgeVec);

    private:
        int getTileIndex(int x, int y, int z);

        // TODO - Eventually this will be a virtual function so that MHPathFinder can be the parent class
        //  of smart pathfinders like "MHDwarfPathFinder" and "MHFlyingCreaturePathFinder", etc.
        int getTraversibleNeighbors(int x, int y, int z, std::vector<Neighbor> &neighbors);

        void removeEdgesAt(int x, int y, int z);
        void addEdgesAt(int x, int y, int z);

    private:
        int _width, _height, _depth;

        Graph *_graph;
        PredecessorMap *_pMap;
        DistanceMap *_dMap;

        // The rationalization for using vector here is that vector actually creates a packed structure of
        //   bits when boolean typed, as opposed to just malloc-ing a boolean array, which uses far more memory
        //   than is necessary.
        std::vector<bool> *_traversibleMap;
};

extern void testMHPathFinder();

#endif
