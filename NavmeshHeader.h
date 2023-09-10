#pragma once

#include "extdll.h"

#include <vector>

class Navmesh;

typedef Navmesh* (*PFN_GET_CLOSEST_NAVMESH)(const Vector&, float);
typedef bool (*PFN_FIND_SHORTEST_PATH)(const Vector&, const Vector&, Navmesh*, Navmesh*, float, int, std::vector<Vector>&);
typedef bool (*PFN_IS_DIRECTION_WALKABLE)(const Vector& , const Vector& , float , float , Vector &);