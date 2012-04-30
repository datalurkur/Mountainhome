#include "LiquidFlow.h"


LiquidSource::LiquidSource(const LiquidSource &other): _point(other._point), _glob(other._glob) {}
LiquidSource::LiquidSource(const Vector3 &point, LiquidGlob *glob): _point(point), _glob(glob) {}

const Vector3 &LiquidSource::point() { return _point; }
LiquidGlob * LiquidSource::glob() { return _glob; }

bool LiquidSource::operator<(const LiquidSource &other) const {
    return (_glob->getSurfaceLevel() > other._glob->getSurfaceLevel());
}

LiquidFlow::LiquidFlow(const LiquidFlow &other): LiquidSource(other._point, other._glob), _sources(other._sources) {}
LiquidFlow::LiquidFlow(const Vector3 &point, LiquidGlob *glob): LiquidSource(point, glob) {}

void LiquidFlow::addSource(const Vector3 &point, LiquidGlob *glob) {
    _sources.push_back(LiquidSource(point, glob));
}

void LiquidFlow::addSources(const LiquidSourceList &sources) { _sources = sources; }
void LiquidFlow::getSources(LiquidSourceList &sources) { sources = _sources; }
