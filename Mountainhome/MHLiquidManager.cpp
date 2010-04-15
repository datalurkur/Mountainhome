#include "MHLiquidManager.h"

MHLiquidManager::MHLiquidManager(MHTerrain *terrain): _terrain(terrain) {

}

MHLiquidManager::~MHLiquidManager() {

}

void MHLiquidManager::SetupBindings() {
    Class = rb_define_class("MHLiquidManager", rb_cObject);
}

