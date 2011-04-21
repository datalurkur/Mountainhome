#include "MHPathFinder.h"

MHPathFinder::MHPathFinder(int width, int height, int depth): _width(width), _height(height), _depth(depth) {
    int size = width*height*depth;

    _graph = new Graph(size);
    _pMap = new PredecessorMap(num_vertices(*_graph));
    _dMap = new DistanceMap(num_vertices(*_graph));
    _traversibleMap = new std::vector<bool>(size, true);
}

MHPathFinder::~MHPathFinder() {
    delete _graph;
    delete _pMap;
    delete _dMap;
    delete _traversibleMap;
}

// This function is called when a tile is set to non-empty
void MHPathFinder::tileBlocked(int x, int y, int z) {
    int index = getTileIndex(x, y, z);
    if((*_traversibleMap)[index]) {
        // This tile was previously traversible, remove any edges that include it as a vertex
        removeEdgesAt(x, y, z);
        (*_traversibleMap)[index] = false;

        // Add any edges that might result from the tile above
        addEdgesAt(x, y, z+1);
    }
}

// This function is called when a tile is set as empty
void MHPathFinder::tileUnblocked(int x, int y, int z) {
    int index = getTileIndex(x, y, z);
    if(!(*_traversibleMap)[index]) {
        // First, unblocking this tile might remove ground upon which other edges sit, so remove those
        removeEdgesAt(x, y, z+1);

        // This tile was previously blocked, add new edges to any traversible nearby vertices
        (*_traversibleMap)[index] = true;
        addEdgesAt(x, y, z);
    }
}

// Set a range of z-values within a column to be blocked
void MHPathFinder::zRangeBlocked(int x, int y, int start_z, int end_z) {
    for(int c=start_z; c<=end_z; c++) {
        int index = getTileIndex(x, y, c);

        removeEdgesAt(x, y, c);

        if(c == end_z && (*_traversibleMap)[index]) {
            (*_traversibleMap)[index] = false;
            addEdgesAt(x, y, end_z+1);
        }
        else {
            (*_traversibleMap)[index] = false;
        }
    }
}

// Set a range of z-values within a column to be empty
void MHPathFinder::zRangeUnblocked(int x, int y, int start_z, int end_z) {
    int prev_z = end_z+1;

    for(int c=end_z; c<=start_z; c--) {
        int index = getTileIndex(x, y, c);

        removeEdgesAt(x, y, prev_z);

        if(c == start_z && !(*_traversibleMap)[index]) {
            (*_traversibleMap)[index] = true;
            addEdgesAt(x, y, c);
        }
        else {
            (*_traversibleMap)[index] = true;
        }

        prev_z = c;
    }
}

void MHPathFinder::removeEdgesAt(int x, int y, int z) {
    int index = getTileIndex(x, y, z);
    std::vector<Neighbor> neighbors;
    if(getTraversibleNeighbors(x, y, z, neighbors) > 0) {
        VertexDescriptor thisVert = vertex(index, *_graph);
        std::vector<Neighbor>::iterator itr;
        for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
            Neighbor thisNeighbor = *itr;
            VertexDescriptor thatVert = vertex(thisNeighbor.first, *_graph);
            remove_edge(thisVert, thatVert, *_graph);
        }
    }
}

void MHPathFinder::addEdgesAt(int x, int y, int z) {
    int index = getTileIndex(x, y, z);
    std::vector<Neighbor> neighbors;
    if(getTraversibleNeighbors(x, y, z, neighbors) > 0) {
        VertexDescriptor thisVert = vertex(index, *_graph);
        std::vector<Neighbor>::iterator itr;
        for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
            Neighbor thisNeighbor = *itr;
            VertexDescriptor thatVert = vertex(thisNeighbor.first, *_graph);
            add_edge(thisVert, thatVert, thisNeighbor.second, *_graph);
        }
    }
}

// Set the start location for pathing and calculate the predecessor and distance maps
void MHPathFinder::setStartPosition(int x, int y, int z) {
    int startIndex = getTileIndex(x, y, z);
    VertexDescriptor startVertex = vertex(startIndex, *_graph);

    _pMap->clear();
    _dMap->clear();
    dijkstra_shortest_paths(*_graph, startVertex, predecessor_map(&(*_pMap)[0]).distance_map(&(*_dMap)[0]));
}

// Fetch the path to the destination using the predecessor map
int MHPathFinder::getPathTo(int x, int y, int z, std::stack<Vector3> &path) {
    int endIndex = getTileIndex(x, y, z);

    VertexDescriptor vert = vertex(endIndex, *_graph);
    int distance = (*_dMap)[vert];
    if((*_pMap)[vert] != vert) {
        for(; (*_dMap)[vert] != 0; vert = (*_pMap)[vert]) {
            path.push(getIndexTile(vert));
        }
    }

    return distance;
}

void MHPathFinder::getClosestPath(std::stack<Vector3> destinations, std::stack<Vector3> &path) {
    int shortestPath = std::numeric_limits<int>::max();
    while(!destinations.empty()) {
        Vector3 dest = destinations.top();

        std::stack<Vector3> newPath;
        int distance = getPathTo(dest[0], dest[1], dest[2], newPath);

        if(distance < shortestPath) {
            path = newPath;
            shortestPath = distance;
        }

        destinations.pop();
    }
}

void MHPathFinder::getFirstPath(std::stack<Vector3> destinations, std::stack<Vector3> &path) {
    std::stack<Vector3>destinations::iterator itr = destinations.begin();
    for(; itr != destinations.end(); itr++) {
        getPathTo((*itr)[0], (*itr)[1], (*itr)[2], path);
        if(path.size > 0) { break; }
    }
}

// Check to see if a path exists to the destination
bool MHPathFinder::isPathBlocked(int x, int y, int z) {
    int index = getTileIndex(x, y, z);
    VertexDescriptor endVertex = vertex(index, *_graph);
    if((*_pMap)[endVertex] == endVertex) { return true; }
    else { return false; }
}

// Pack a vector3 into an index
int MHPathFinder::getTileIndex(int x, int y, int z) {
    if(x<0 || x>=_width || y<0 || y>=_height || z<0 || z>=_depth) {
        return -1;
    }
    return (z * _width * _height) + (y * _width) + x;
}

// Unpack an index into its corresponding vector
Vector3 MHPathFinder::getIndexTile(int index) const {
    int x,y,z;
    int z_size = _width * _height;

    z = index / z_size;
    index -= (z * z_size);

    y = index / _width;
    index -= (y * _width);

    x = index;

    return Vector3(x, y, z);
}

int MHPathFinder::getTraversibleNeighbors(int x, int y, int z, std::vector<Neighbor> &neighbors) {
    // Don't bother if this tile has no solid ground beneath it
    int groundIndex = getTileIndex(x, y, z-1);
    if(groundIndex == -1 || (*_traversibleMap)[groundIndex]) {
        return 0;
    }
    // Also don't bother if this tile is filled
    int thisIndex = getTileIndex(x, y, z);
    if(thisIndex == -1 || !(*_traversibleMap)[thisIndex]) {
        return 0;
    }

    // Check the immediately surrounding neighbors
    for(int h=-1; h<=1; h++) {
        for(int j=-1; j<=1; j++) {
            if(h==0 && j==0) { continue; }

            int local_x = x+h,
                local_y = y+j,
                local_z = z;
            bool isDiagonal = false;
            if(abs(h) == abs(j)) {
                isDiagonal = true;
            }

            int thisLevelIndex = getTileIndex(local_x, local_y, local_z);
            if(thisLevelIndex == -1) { continue; }
            else if((*_traversibleMap)[thisLevelIndex]) {
                // This neighbor is empty, check the z-level below it to determine if this is a valid node
                int lowerLevelIndex = getTileIndex(local_x, local_y, local_z-1);
                if(lowerLevelIndex == -1) { continue; }
                else if((*_traversibleMap)[lowerLevelIndex]) {
                    // The space below this neighbor is empty, can we move down a step to it?
                    int subLowerIndex = getTileIndex(local_x, local_y, local_z-2);
                    if(subLowerIndex == -1) { continue; }
                    else if(!(*_traversibleMap)[subLowerIndex]) {
                        // We can step down to the lower level, add an edge here
                        if(isDiagonal) {
                            neighbors.push_back(std::make_pair(lowerLevelIndex, CORNER_WEIGHT));
                        }
                        else {
                            neighbors.push_back(std::make_pair(lowerLevelIndex, DIAGONAL_WEIGHT));
                        }
                    }
                }
                else {
                    // The space below this neighbor is filled, add an edge here
                    if(isDiagonal) {
                        neighbors.push_back(std::make_pair(thisLevelIndex, DIAGONAL_WEIGHT));
                    }
                    else {
                        neighbors.push_back(std::make_pair(thisLevelIndex, NORMAL_WEIGHT));
                    }
                }
            }
            else {
                // This neighbor is filled, check the z-level above it to determine if it can be walked on
                int upperLevelIndex = getTileIndex(local_x, local_y, local_z+1);
                if(upperLevelIndex == -1) { continue; }
                else if((*_traversibleMap)[upperLevelIndex]) {
                    // We can step up to the upper level, add an edge here
                    if(isDiagonal) {
                        neighbors.push_back(std::make_pair(upperLevelIndex, CORNER_WEIGHT));
                    }
                    else {
                        neighbors.push_back(std::make_pair(upperLevelIndex, DIAGONAL_WEIGHT));
                    }
                }
            }
        }
    }
    return neighbors.size();
}

void MHPathFinder::getEdges(std::vector<Edge> &edgeVec) {
    graph_traits<Graph>::edge_iterator ei, ei_end;
    for(tie(ei, ei_end) = edges(*_graph); ei != ei_end; ++ei) {
        VertexDescriptor u = source(*ei, *_graph),
                         v = target(*ei, *_graph);
        edgeVec.push_back(Edge(getIndexTile(u),getIndexTile(v)));
    }
}

// Note: This is a *lot* of information to print out.  This is only meant to be used as a small-scale debugging tool
std::ostream& operator<<(std::ostream &lhs, const MHPathFinder &rhs) {
    lhs << "MHPathFinder (\n";

    Graph *graph = rhs.getGraph();

    graph_traits<Graph>::edge_iterator ei, ei_end;
    property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, *graph);

    for(tie(ei, ei_end) = edges(*graph); ei != ei_end; ++ei) {
        EdgeDescriptor e = *ei;
        VertexDescriptor u, v;
        u = source(e, *graph);
        v = target(e, *graph);
        lhs << rhs.getIndexTile(u) << " -> " << rhs.getIndexTile(v) << " (" << get(weightmap, e) << ")\n";
    }
    lhs << ")\n";
    return lhs;
}

// Quick test case
void testMHPathFinder() {
    /* Set our world's heightmap to be something like this:
        1 1 1 1 1
        3 2 1 1 1
        4 3 3 3 1
        3 2 1 1 1
        1 1 1 1 1
       Expecting pathing to look something like this:
        S * * * *
        * 1 * * *
        * 2 * * *
        * 3 * * *
        E * * * *
    */
    int grid[5][5];
    for(int h=0; h<5; h++) {
        for(int j=0; j<5; j++) {
            grid[h][j]=1;
        }
    }
    grid[0][1] = 3;
    grid[0][2] = 4;
    grid[0][3] = 3;
    grid[1][1] = 2;
    grid[1][2] = 3;
    grid[1][3] = 2;
    grid[2][2] = 3;
    grid[3][2] = 3;

    MHPathFinder pathFinder(5, 5, 5);
    for(int h=0; h<5; h++) {
        for(int j=0; j<5; j++) {
            for(int k=grid[h][j]; k<5; k++) {
                pathFinder.tileUnblocked(h,j,k);
            }
        }
    }

    Info("Graph information: " << pathFinder);

    std::stack<Vector3> path;
    pathFinder.setStartPosition(0, 0, 1);
}
