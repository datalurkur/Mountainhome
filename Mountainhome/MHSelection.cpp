#include "MHSelection.h"

MHSelection::MHSelection() {}

MHSelection::~MHSelection() {}

void MHSelection::clear() {
    _selectedActors.clear();
    _selectedTiles.clear();
}

void MHSelection::append(MHActor *actor) { _selectedActors.push_back(actor); }
void MHSelection::append(Vector3 &tile)  { _selectedTiles.push_back(tile);   }

int MHSelection::numSelectedActors() { return _selectedActors.size(); }
int MHSelection::numSelectedTiles()  { return _selectedTiles.size();  }

std::list <MHActor*> &MHSelection::getSelectedActors() { return _selectedActors; }
std::list <Vector3> &MHSelection::getSelectedTiles()   { return _selectedTiles;  }
