#include "MHPathFinder.h"

MHPathFinder::MHPathFinder(int width, int height, int depth): _width(width), _height(height), _depth(depth) {
    int size = width*height*depth;

    _graph = new Graph(size);
    _pMap = new PredecessorMap(num_vertices(*_graph));
    _dMap = new DistanceMap(num_vertices(*_graph));
    _nodeMap = new std::vector<PathNode>(size);
}

MHPathFinder::~MHPathFinder() {
    delete _graph;
    delete _pMap;
    delete _dMap;
    delete _nodeMap;
}

void MHPathFinder::setTileClosed(int x, int y, int z) {
    int index = getTileIndex(x, y, z);

    // Get the neighbors adjacent to this node
    std::vector<Vector3> neighbors;
    getNeighbors(x, y, z, neighbors);

    // Remove all of their edges
    std::vector<Vector3>::iterator itr;
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        removeEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }

    (*_nodeMap)[index].setClosed();

    // Now, we need to refresh the paths adjacent to this node
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        addEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }
}

void MHPathFinder::setTileOpen(int x, int y, int z) {
    int index = getTileIndex(x, y, z);

    // Get the neighbors adjacent to this node
    std::vector<Vector3> neighbors;
    getNeighbors(x, y, z, neighbors);

    // Remove all of their edges
    std::vector<Vector3>::iterator itr;
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        removeEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }

    // Set the node to open
    (*_nodeMap)[index].setOpen();

    // Now, we need to refresh the paths adjacent to this node
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        addEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }
}

void MHPathFinder::setTilePathable(int x, int y, int z) {
    int index = getTileIndex(x, y, z);

    // Get the neighbors adjacent to this node
    std::vector<Vector3> neighbors;
    getNeighbors(x, y, z, neighbors);

    // Remove all of their edges
    std::vector<Vector3>::iterator itr;
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        removeEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }

    // Set the node to pathable
    (*_nodeMap)[index].setPathable();

    // Now, we need to refresh the paths adjacent to this node
    for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
        addEdgesAt((*itr)[0], (*itr)[1], (*itr)[2]);
    }
}

void MHPathFinder::removeEdgesAt(int x, int y, int z) {
    std::vector<Neighbor> neighbors;
    if(getPathableNeighbors(x, y, z, neighbors) > 0) {
        int index = getTileIndex(x,y,z);
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
    std::vector<Neighbor> neighbors;
    if(getPathableNeighbors(x, y, z, neighbors) > 0) {
        int index = getTileIndex(x,y,z);
        VertexDescriptor thisVert = vertex(index, *_graph);
        std::vector<Neighbor>::iterator itr;
        for(itr = neighbors.begin(); itr != neighbors.end(); itr++) {
            Neighbor thisNeighbor = *itr;
            VertexDescriptor thatVert = vertex(thisNeighbor.first, *_graph);
            add_edge(thisVert, thatVert, thisNeighbor.second, *_graph);
        }
    }
}

int MHPathFinder::getNeighbors(int x, int y, int z, std::vector<Vector3> &neighbors) {
    for(int i=-1; i<=1; i++) {
        for(int j=-1; j<=1; j++) {
            for(int k=-1; k<=1; k++) {
                if(i==0 && j==0 && k==0) {
                    continue;
                } else {
                    int local_x = x+i,
                        local_y = y+j,
                        local_z = z+k;
                    if(getTileIndex(local_x, local_y, local_z) == -1) { continue; }
                    neighbors.push_back(Vector3(local_x, local_y, local_z));
                }
            }
        }
    }
}

int MHPathFinder::getPathableNeighbors(int x, int y, int z, std::vector<Neighbor> &neighbors) {
    int index = getTileIndex(x, y, z);
    if(!(*_nodeMap)[index].isPathable()) { return 0; }

    // Iterate over the immediate neighbors
    for(int i=-1; i<=1; i++) {
        for(int j=-1; j<=1; j++) {
            // Skip the center node (the node we're trying to find neighbors for)
            if(i==0 && j==0) { continue; }

            // Get the local coordinates for this neighbor
            int local_x = x+i,
                local_y = y+j;
            bool isDiagonal = (abs(i) == abs(j));
            int weight = (isDiagonal ? DIAGONAL_WEIGHT : NORMAL_WEIGHT);

            // Get the local node index and continue if OOB
            int localIndex = getTileIndex(local_x, local_y, z);
            if(localIndex == -1) { continue; }

            // Find a node for this x,y pair that's pathable or continue to the next x,y pair
            int local_z = z;
            if(!(*_nodeMap)[localIndex].isPathable()) {
                if((*_nodeMap)[localIndex].isClosed()) {
                    int aboveIndex = getTileIndex(x, y, z+1);
                    if(aboveIndex == -1 || (*_nodeMap)[aboveIndex].isClosed()) { continue; }
                    local_z += 1;
                }
                else {
                    local_z -= 1;
                }

                localIndex = getTileIndex(local_x, local_y, local_z);
                if(localIndex == -1 || !(*_nodeMap)[localIndex].isPathable()) { continue; }
                weight = (isDiagonal ? CORNER_WEIGHT : DIAGONAL_WEIGHT);
            }

            // Check to make sure the cardinal tiles don't block diagonal movement
            if(isDiagonal) {
                int sideIndexA = getTileIndex(x, local_y, local_z),
                    sideIndexB = getTileIndex(local_x, y, local_z);
                if((*_nodeMap)[sideIndexA].isClosed() && (*_nodeMap)[sideIndexB].isClosed()) { continue; }
            }

            // If we've made it this far, this neighbor is pathable
            neighbors.push_back(std::make_pair(localIndex, weight));
        }
    }
    return neighbors.size();
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
    while(!destinations.empty()) {
        Vector3 dest = destinations.top();
        getPathTo(dest[0], dest[1], dest[2], path);
        if(path.size() > 0) { break; }
        destinations.pop();
    }
}

// Check to see if a path exists to the destination
bool MHPathFinder::isPathBlocked(int x, int y, int z) {
    VertexDescriptor endVertex = vertex(getTileIndex(x, y, z), *_graph);
    return ((*_pMap)[endVertex] == endVertex);
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
            int lowest_k = grid[h][j];
            pathFinder.setTilePathable(h,j,lowest_k);
            for(int k=lowest_k+1; k<5; k++) {
                pathFinder.setTileOpen(h,j,k);
            }
        }
    }

    Info("Graph information: " << pathFinder);

    std::stack<Vector3> path;
    pathFinder.setStartPosition(0, 0, 1);
}
