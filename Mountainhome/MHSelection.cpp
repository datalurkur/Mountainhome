#include "MHSelection.h"

MHSelection::MHSelection() {}

MHSelection::~MHSelection() {}

void MHSelection::clear() {
    _selectedActors.clear();
    _selectedVoxels.clear();
}

void MHSelection::append(MHActor *actor) { _selectedActors.push_back(actor); }
void MHSelection::append(Vector3 &voxel)  { _selectedVoxels.push_back(voxel);   }

void MHSelection::remove(Vector3 voxel)  { _selectedVoxels.remove(voxel);      }

int MHSelection::numSelectedActors() { return _selectedActors.size(); }
int MHSelection::numSelectedVoxels()  { return _selectedVoxels.size();  }

std::list <MHActor*> &MHSelection::getSelectedActors() { return _selectedActors; }
std::list <Vector3> &MHSelection::getSelectedVoxels()   { return _selectedVoxels;  }
