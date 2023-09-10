#pragma once

#include <vector>

class Navmesh;
class NavmeshBridge;
class Vector;

class PathResult
{
public:
	void Insert(Navmesh* pMesh, NavmeshBridge* pBridge);

	const std::pair<Navmesh*, NavmeshBridge*>& Get(size_t i)
	{
		return m_path[i];
	}

	std::vector<Vector> Smooth(const Vector& sp, const Vector& ep, int maxPts = 0);

	size_t Size() { return m_path.size(); }

private:

	float TriArea(const Vector& a, const Vector& b, const Vector& c) const;
	bool vequal(const Vector& a, const Vector& b) const;

	std::vector<std::pair<Navmesh*, NavmeshBridge*>> m_path;
};