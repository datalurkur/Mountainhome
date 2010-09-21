#include "MHPath.h"
#include "MHWorld.h"
#include "MHTerrain.h"

struct PathNode {
    PathNode(const Vector3 &nPos, float nCost, float nDist, PathNode *nParent):
        parent(nParent), position(nPos), distance(nDist), cost(nCost) {}

    float score() { return cost + distance; }

    PathNode *parent;
    Vector3 position;
    float distance;
    float cost;

};


#pragma mark Ruby Bindings
void MHPath::SetupBindings() {
    Class = rb_define_class("MHPath", rb_cObject);
	
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHPath::Initialize), 7);
    rb_define_method(Class, "next_step", RUBY_METHOD_FUNC(MHPath::NextStep), 0);
    rb_define_method(Class, "blocked?", RUBY_METHOD_FUNC(MHPath::Blocked), 0);
    rb_define_method(Class, "end_of_path?", RUBY_METHOD_FUNC(MHPath::EndOfPath), 0);
    rb_define_alloc_func(Class, MHPath::Alloc);
}

VALUE MHPath::Initialize(VALUE rSelf, VALUE rWorld, VALUE rSX, VALUE rSY, VALUE rSZ, VALUE rDX, VALUE rDY, VALUE rDZ) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    AssignCObjFromValue(MHWorld, cWorld, rWorld);

    cSelf->initialize(cWorld, Vector3(NUM2INT(rSX), NUM2INT(rSY), NUM2INT(rSZ)),
                              Vector3(NUM2INT(rDX), NUM2INT(rDY), NUM2INT(rDZ)));
    return rSelf;
}

VALUE MHPath::NextStep(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    Vector3 cStep = cSelf->getNextStep();

    VALUE *coords = new VALUE[3];
    coords[0] = INT2NUM(cStep[0]);
    coords[1] = INT2NUM(cStep[1]);
    coords[2] = INT2NUM(cStep[2]);

    return rb_ary_new4(3, coords);
}

VALUE MHPath::Blocked(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    return (cSelf->blocked()? Qtrue : Qfalse);
}

VALUE MHPath::EndOfPath(VALUE rSelf) {
    AssignCObjFromValue(MHPath, cSelf, rSelf);
    return (cSelf->path.empty()? Qtrue : Qfalse);
}

#pragma mark Static functions
void MHPath::GetTraversibleNeighbors(MHTerrain *terrain, const Vector3 &loc, std::stack <Vector3> *neighbors) {
    // NOTE: For the time being, this function *only* pays attention to whether a tile is full or empty,
    //  meaning it does not account for doors, other constructions, organic obstructions, etc

    // Start with the current z-level
    // Info("-Finding neighbors for " << loc);
    for (int xdiff = -1; xdiff <= 1; xdiff++) {
        for (int ydiff = -1; ydiff <= 1; ydiff++) {
            if (xdiff == 0 && ydiff == 0) { continue; }

            int xLoc = loc[0] + xdiff,
                yLoc = loc[1] + ydiff;

            if (xLoc < 0 || xLoc >= terrain->getWidth() || yLoc < 0 || yLoc >= terrain->getHeight()) { continue; }

            // Info("--Checking neighbor at " << (Vector3(xLoc, yLoc, loc[2])));
            // Check to see if the adjacent square is empty
            if (terrain->getTileType(xLoc, yLoc, loc[2]) == TILE_EMPTY) {
                // We can possibly move here or to the tile below this one
                if (terrain->getTileType(xLoc, yLoc, loc[2] - 1) != TILE_EMPTY) {
                    // Info("---Found empty neighbor");
                    neighbors->push(Vector3(xLoc, yLoc, loc[2]));
                }
                else if (terrain->getTileType(xLoc, yLoc, loc[2] - 1) == TILE_EMPTY &&
                   terrain->getTileType(xLoc, yLoc, loc[2] - 2) != TILE_EMPTY) {
                    // Info("---Found adjacent ramp down");
                    neighbors->push(Vector3(xLoc, yLoc, loc[2] - 1));
                }
            }
            else if (terrain->getTileType(xLoc, yLoc, loc[2] + 1) == TILE_EMPTY) {
                // Info("---Found adjacent ramp up");
                // We can't move into a wall, but we can move up a z-level if the tile above is empty
                neighbors->push(Vector3(xLoc, yLoc, loc[2] + 1));
            }
            else {
                // Info("---Adjacent tiles filled");
            }
        }
    }
}

bool MHPath::FindPath(const Vector3 &source, const Vector3 &dest, std::stack <Vector3> *path, MHTerrain *terrain) {
    if(source == dest) { return true; }

    Info("Attempting to find path from " << source << " to " << dest);

    // 0) Initialize the open and closed sets to be empty
    std::list <PathNode*> closedSet;
    std::list <PathNode*> openSet;
    std::list <PathNode*>::iterator itr;

    // 1) Add the source to the open set
    PathNode *origin = new PathNode(source, 0, source.distanceTo(dest), NULL);
    openSet.push_front(origin);

    // 2) While the open set is not empty
    while (!openSet.empty()) {
        // i) Select the node in the open set with the lowest cost
        float lowestCost = FLT_MAX;
        std::list <PathNode*>::iterator cItr;
        PathNode *cPath;

        for (itr = openSet.begin(); itr != openSet.end(); itr++) {
            if (lowestCost > (*itr)->score()) {
                lowestCost = (*itr)->score();
                cItr = itr;
            }
        }

        // ii) Add this node to the closed set
        cPath = (*cItr);
        closedSet.push_front(cPath);
        openSet.erase(cItr);

        // Info("Lowest cost is " << lowestCost << " at location " << cPath->_pos);

        // iii) Is this the destination?
        if (cPath->position == dest) {
            // We have found a path
            // Clear the open list
            clear_list(openSet);

            // Extract the path from the closed list
            PathNode *path_step = cPath;
            while(path_step->position != source) {
                path->push(path_step->position);
                path_step = path_step->parent;
            }

            // Clear the closed list
            clear_list(closedSet);

            // Return success
            return true;
        }
        else {
            // For each immediately traversible node not in the closed set
            std::stack <Vector3> neighbors;
            GetTraversibleNeighbors(terrain, cPath->position, &neighbors);

            while (!neighbors.empty()) {
                Vector3 neighborPosition = neighbors.top();
                neighbors.pop();

                // Is this neighbor in the closed set?
                bool alreadyClosed = false;
                for (itr = closedSet.begin(); itr != closedSet.end(); itr++) {
                    if ((*itr)->position == neighborPosition) {
                        // Info("Neighbor " << neighborPosition << " already in closed set");
                        alreadyClosed = true;
                        break;
                    }
                }
                if (alreadyClosed) { continue; }

                // a) Compute direct distance to dest
                float neighborDist = neighborPosition.distanceTo(dest);
                // b) Compute cost based on distance already travelled
                float neighborCost = cPath->cost + cPath->position.distanceTo(neighborPosition);
                // c) Compute the score based on the addition of these two metrics
                float neighborScore = neighborDist + neighborCost;
                
                // d) Already part of the open set?
                for (itr = openSet.begin(); itr != openSet.end(); itr++) {
                    if((*itr)->position == neighborPosition) {
                        if ((*itr)->score() > neighborScore) {
                            // Relink path to this node if the cost is cheaper
                            (*itr)->parent   = cPath;
                            (*itr)->distance = neighborDist;
                            (*itr)->cost     = neighborCost;
                        }
                        break;
                    }
                }

                // e) Add to the open set, linked to the originating node
                if (itr == openSet.end()) {
                    PathNode *neighbor = new PathNode(neighborPosition, neighborCost, neighborDist, cPath);
                    openSet.push_front(neighbor);
                }
            }
        }
    }
    Info("Suitable path not found");

    // Make sure we clean up after ourselves
    // Clear out the closed list
    clear_list(closedSet);
    clear_list(openSet);

    // If we reach this point, there is no path to the destination, return failure
    return false;
}

#pragma mark Instance Functions

MHPath::MHPath() { }

MHPath::~MHPath() { }

void MHPath::initialize(MHWorld *world, Vector3 source, Vector3 dest) {
    accessible = FindPath(source, dest, &path, world->getTerrain());
}

bool MHPath::blocked() {
    return !accessible;
}

bool MHPath::endOfPath() {
    return path.empty();
}

Vector3 MHPath::getNextStep() {
    Vector3 nStep = path.top();
    path.pop();
    return nStep;
}
