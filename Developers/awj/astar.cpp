struct PathNode {
    Vector3 _pos;
    int _cost;
    int _dist;
    PathNode *_parent;

    PathNode() {}
    PathNode(Vector3 pos, int cost, int dist, PathNode *parent): _pos(pos), _cost(cost), _dist(dist), _parent(parent) {}
    ~PathNode() {}

    int score() { return _cost + _dist; }
};

// Stub functions which are basically functionality that might partially exist elsewhere
int distance(Vector3 source, Vector3 dest) = 0;
void getTraversibleNeighbors(Vector3 loc, std::stack <Vector3> &neighbors) = 0;

bool findPath(Vector3 source, Vector3 dest, std::stack <Vector3> &path) {
    // 0) Initialize the open and closed sets to be empty
    std::list <PathNode*> closedSet;
    std::list <PathNode*> openSet;
    std::list <PathNode*>::iterator itr;

    // 1) Add the destination to the open set
    PathNode *origin = new PathNode(dest, 0, distance(source, dest), NULL);
    openSet.push_front(origin);

    // 2) While the open set is not empty
    while(!openSet.empty()) {
        // i) Select the node in the open set with the lowest cost
        int lowestCost = -1;
        std::list <PathNode*>::iterator current;

        for(itr = openSet.begin(); itr != openSet.end(); itr++) {
            if(lowestCost == -1) {
                lowestCost = (*itr)->score();
                current = itr;
            }
            else {
                if(lowestCost > (*itr)->score()) {
                    lowestCost = (*itr)->score();
                    current = itr;
                }
            }
        }

        // ii) Add this node to the closed set
        closedSet.push_front(*current);
        openSet.erase(current);

        // iii) Is this the origin?
        if((*current)->_pos == source) {
            // We have found a path
            // Clear the open list
            for(itr = openSet.begin(); itr != openSet.end(); itr++) {
                delete (*itr);
            }
            openSet.erase(openSet.begin(), openSet.end());

            // Extract the path from the closed list
            PathNode *path_step = (*current);
            while(path_step->_pos != dest) {
                path.push(path_step->_pos);
                path_step = path_step->_parent;
            }

            // Clear the closed list
            for(itr = openSet.begin(); itr != openSet.end(); itr++) {
                delete (*itr);
            }
            closedSet.erase(closedSet.begin(), closedSet.end());

            // Return success
            return true;
        }
        else {
            // For each immediately traversible node not in the closed set
            std::stack <Vector3> neighbors;
            getTraversibleNeighbors((*current)->_pos, &neighbors);

            while(!neighbors.empty()) {
                Vector3 neighborPosition = neighbors.pop();
                // a) Compute direct distance to source
                int neighborDist = distance(neighborPosition, source);
                // b) Compute cost based on distance already travelled
                int neighborCost = (*current)->_cost + distance((*current)->_pos, neighborPosition);
                // c) Compute the score based on the addition of these two metrics
                int neighborScore = neighborDist + neighborCost;
                
                // d) Already part of the open set?
                for(itr = openSet.begin(); itr != openSet.end(); itr++) {
                    if(((*itr)->_pos == neighborPosition) && ((*itr)->score() > neighborScore))
                        // Yes -> Relink path to this node if the cost is cheaper
                        (*itr)->_cost = neighborCost;
                        (*itr)->_dist = neighborDist;
                        (*itr)->_parent = (*current);

                        break;
                    }
                }

                // No  -> Add to the open set, linked to the originating node
                if(itr == openSet.end()) {
                    PathNode *neighbor = new PathNode(neighborPosition, neighborCost, neighborDist, (*current));
                    openSet.push_front(neighbor);
                }
            }
        }
    }

    // Make sure we clean up after ourselves
    // Clear out the closed list
    for(itr = closedSet.begin(); itr != closedSet.end(); itr++) {
        delete (*itr);
    }
    closedSet.erase(closedSet.begin(), closedSet.end());

    // Clear out the open list
    for(itr = openSet.begin(); itr != openSet.end(); itr++) {
        delete (*itr);
    }
    openSet.erase(closedSet.begin(), closedSet.end());

    // If we reach this point, there is no path to the destination, return failure
    return false;
}
