/*
    PathFinder.cpp
    Mountainhome

    Created by: Andrew Jean
    Created on: July 13, 2010
    Last Modified: July 14, 2010
    Copyright 2010 Mountainhome Project. All rights reserved.
*/

#include "PathFinder.h"

int distance(Vector3 source, Vector3 dest) {
    Vector3 diff = source - dest;
    return diff.length();
}

void getTraversibleNeighbors(MHTerrain *terrain, Vector3 loc, std::stack <Vector3> *neighbors) {
    // NOTE: For the time being, this function *only* pays attention to whether a tile is full or empty,
    //  meaning it does not account for doors, other constructions, organic obstructions, etc

    // Start with the current z-level
    Info("Finding neighbors for " << loc);
    for(int xdiff = -1; xdiff <= 1; xdiff++) {
        for(int ydiff = -1; ydiff <= 1; ydiff++) {
            if(xdiff==0 && ydiff==0) { continue; }

            int xLoc = loc[0] + xdiff,
                yLoc = loc[1] + ydiff;
            Info("Checking neighbor at " << (Vector3(xLoc, yLoc, loc[2])));
            // Check to see if the adjacent square is empty
            if(terrain->getTile(xLoc, yLoc, loc[2])==TILE_EMPTY) {
                // We can possibly move here or to the tile below this one
                if(terrain->getTile(xLoc, yLoc, loc[2]-1)!=TILE_EMPTY) {
                    Info("Found empty neighbor");
                    neighbors->push(Vector3(xLoc, yLoc, loc[2]));
                }
                else if(terrain->getTile(xLoc, yLoc, loc[2]-1)==TILE_EMPTY &&
                   terrain->getTile(xLoc, yLoc, loc[2]-2)!=TILE_EMPTY) {
                    Info("Found adjacent ramp down");
                    neighbors->push(Vector3(xLoc, yLoc, loc[2]-1));
                }
            }
            else if(terrain->getTile(xLoc, yLoc, loc[2]+1)==TILE_EMPTY) {
                Info("Found adjacent ramp up");
                // We can't move into a wall, but we can move up a z-level if the tile above is empty
                neighbors->push(Vector3(xLoc, yLoc, loc[2]+1));
            }
            else {
                Info("Adjacent tiles filled");
            }
        }
    }
}

bool findPath(Vector3 source, Vector3 dest, std::stack <Vector3> *path, MHTerrain *terrain) {
    Info("Attempting to find path from " << source << " to " << dest);

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
        std::list <PathNode*>::iterator cItr;
        PathNode *cPath;

        for(itr = openSet.begin(); itr != openSet.end(); itr++) {
            if(lowestCost == -1) {
                lowestCost = (*itr)->score();
                cItr = itr;
            }
            else {
                if(lowestCost > (*itr)->score()) {
                    lowestCost = (*itr)->score();
                    cItr = itr;
                }
            }
        }

        // ii) Add this node to the closed set
        cPath = (*cItr);
        closedSet.push_front(cPath);
        openSet.erase(cItr);

        Info("Lowest cost is " << lowestCost << " at location " << cPath->_pos);

        // iii) Is this the origin?
        if(cPath->_pos == source) {
            // We have found a path
            // Clear the open list
            for(itr = openSet.begin(); itr != openSet.end(); itr++) {
                delete (*itr);
            }
            openSet.erase(openSet.begin(), openSet.end());

            // Extract the path from the closed list
            PathNode *path_step = cPath;
            while(path_step->_pos != dest) {
                path->push(path_step->_pos);
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
            getTraversibleNeighbors(terrain, cPath->_pos, &neighbors);

            while(!neighbors.empty()) {
                Vector3 neighborPosition = neighbors.top();
                neighbors.pop();
                // a) Compute direct distance to source
                int neighborDist = distance(neighborPosition, source);
                // b) Compute cost based on distance already travelled
                int neighborCost = cPath->_cost + distance(cPath->_pos, neighborPosition);
                // c) Compute the score based on the addition of these two metrics
                int neighborScore = neighborDist + neighborCost;
                
                // d) Already part of the open set?
                for(itr = openSet.begin(); itr != openSet.end(); itr++) {
                    if((*itr)->_pos == neighborPosition) {
                        if((*itr)->score() > neighborScore) {
                            // Relink path to this node if the cost is cheaper
                            (*itr)->_cost = neighborCost;
                            (*itr)->_dist = neighborDist;
                            (*itr)->_parent = cPath;
                        }
                        break;
                    }
                }

                // e) Add to the open set, linked to the originating node
                if(itr == openSet.end()) {
                    PathNode *neighbor = new PathNode(neighborPosition, neighborCost, neighborDist, cPath);
                    openSet.push_front(neighbor);
                }
            }
        }
    }
    Info("Suitable path not found");

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
