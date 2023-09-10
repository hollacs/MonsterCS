#pragma once

#include "extdll.h"

#include <vector>

class Steer
{
public:
	Steer() : m_pEntity(nullptr) {}
	Steer(edict_t* pEntity) : m_pEntity(pEntity) {}

	//void Seek(const Vector& target, float slowingRadius);
	//void Flee(const Vector& target);
	//void Evade(edict_t* pEntity);
	//void Pursuit(edict_t* pEntity);

	void SetEntity(edict_t* pEntity) { m_pEntity = pEntity; }
	void Reset();
	void Update() const;
	void AddForce(const Vector& force);

	Vector Seek(const Vector& target, float slowingRadius=0) const;
	Vector Flee(const Vector& target) const;
	Vector Evade(edict_t* pEntity) const;
	Vector Pursuit(edict_t* pEntity) const;
	Vector AvoidMonsters(float minTimeToCollision) const;
	Vector Wander(float dt) const;
	Vector StayWithinMesh() const;
	Vector FollowPath(const Vector& sp, const Vector& ep, const std::vector<Vector>& path, int maxFollows) const;

private:

	Vector Truncate(const Vector& vec, float max) const;

	Vector AvoidCloseMonsters(float minSeparationDistance) const;
	float ComputeNearestApproachPositions(edict_t* pOther, float time, Vector& myFinal, Vector& otherFinal) const;
	float PredictNearestApproachTime(edict_t* pOther) const;

	float DistPointSegment2D(const Vector2D& p, const Vector2D& sp0, const Vector2D& sp1, Vector2D& out) const;

	float ScalarRandomWalk(float initial, float walkspeed, float min, float max) const;
	Vector PerpendicularComponent(const Vector& vec, const Vector& unitBasis) const;
	Vector GetOrigin(edict_t* pEntity) const;

	edict_t* m_pEntity;
	Vector m_steering;
};