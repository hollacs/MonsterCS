#include "Steer.h"
#include "NavmeshHeader.h"
#include "Utils.h"

#include "meta_api.h"

extern PFN_IS_DIRECTION_WALKABLE g_pfnIsDirectionWalkable;

void Steer::Reset()
{
	m_steering = Vector();
}

void Steer::Update() const
{
	Vector clippedForce = Truncate(m_steering, 50.0f);
	Vector newAcceleration = clippedForce / 1.0f;

	Vector newVelocity = m_pEntity->v.clbasevelocity;
	newVelocity = newVelocity + newAcceleration;
	newVelocity = Truncate(newVelocity, m_pEntity->v.maxspeed);
	newVelocity.z = m_pEntity->v.velocity.z;

	m_pEntity->v.velocity = newVelocity;
	m_pEntity->v.clbasevelocity = newVelocity;

	Vector angle;
	VEC_TO_ANGLES(newVelocity, angle);
	angle.x = 0;
	m_pEntity->v.angles = angle;

	MOVE_TO_ORIGIN(m_pEntity, GetOrigin(m_pEntity) + newVelocity, 0.5, 1);
}

void Steer::AddForce(const Vector& force)
{
	m_steering = m_steering + force;
}

Vector Steer::Seek(const Vector& target, float slowingRadius) const
{
	Vector force;
	Vector desired = target - GetOrigin(m_pEntity);

	float distance = desired.Length();
	desired.Normalize();

	if (distance <= slowingRadius)
		desired = desired * (m_pEntity->v.maxspeed * distance / slowingRadius);
	else
		desired = desired * m_pEntity->v.maxspeed;

	force = desired - m_pEntity->v.clbasevelocity;

	return force;
}

Vector Steer::Pursuit(edict_t* pEntity) const
{
	Vector distance = GetOrigin(pEntity) - GetOrigin(m_pEntity);

	float updates = distance.Length() / m_pEntity->v.maxspeed;

	Vector futurePos = GetOrigin(pEntity) + pEntity->v.velocity * updates;

	return Seek(futurePos);
}

Vector Steer::AvoidMonsters(float minTimeToCollision) const
{
	Vector separation = AvoidCloseMonsters(0);
	if (separation != Vector())
		return separation;

	float steer = 0;
	edict_t* pThreat = NULL;

	float minTime = minTimeToCollision;
	Vector origin = GetOrigin(m_pEntity);

	Vector threatPosAtNearestApproach;
	Vector ourPosAtNearestApproach;

	edict_t* pSearchEnt = NULL;
	while ((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, origin, 1000)) != NULL)
	{
		if (pSearchEnt == m_pEntity || FNullEnt(pSearchEnt))
			continue;

		if (pSearchEnt->v.solid == SOLID_SLIDEBOX && pSearchEnt->v.movetype == MOVETYPE_STEP)
		{
			float collisionDangerThreshold = m_pEntity->v.maxs.x * 2;

			float time = PredictNearestApproachTime(pSearchEnt);

			if ((time >= 0) && (time < minTime))
			{
				Vector myFinal, otherFinal;
				if (ComputeNearestApproachPositions(pSearchEnt, time, myFinal, otherFinal) < collisionDangerThreshold)
				{
					minTime = time;
					pThreat = pSearchEnt;
					threatPosAtNearestApproach = otherFinal;
					ourPosAtNearestApproach = myFinal;
				}
			}
		}
	}

	MAKE_VECTORS(m_pEntity->v.angles);
	Vector forward = gpGlobals->v_forward;
	Vector side = gpGlobals->v_right;

	if (pThreat != NULL)
	{
		MAKE_VECTORS(pThreat->v.angles);
		Vector threatForward = gpGlobals->v_forward;

		float parallelness = DotProduct(forward, threatForward);
		float angle = 0.707f;

		if (parallelness < -angle)
		{
			Vector offset = threatPosAtNearestApproach - origin;
			float sideDot = DotProduct(offset, side);
			steer = (sideDot > 0) ? -1.0f : 1.0f;
		}
		else
		{
			if (parallelness > angle)
			{
				Vector offset = GetOrigin(pThreat) - origin;
				float sideDot = DotProduct(offset, side);
				steer = (sideDot > 0) ? -1.0f : 1.0f;
			}
			else
			{
				if (pThreat->v.velocity.Length2D() <= m_pEntity->v.velocity.Length2D())
				{
					float sideDot = DotProduct(side, pThreat->v.velocity);
					steer = (sideDot > 0) ? -1.0f : 1.0f;
				}
			}
		}
	}

	return side * steer;
}

Vector Steer::Wander(float dt) const
{
	float speed = 12.0f * dt;

	float wanderSide = 0.0;
	float wanderFwd = 0.0;

	wanderSide = ScalarRandomWalk(wanderSide, speed, -1, +1);
	wanderFwd = ScalarRandomWalk(wanderFwd, speed, -1, +1);

	MAKE_VECTORS(m_pEntity->v.angles);
	
	return (gpGlobals->v_right * wanderSide) + (gpGlobals->v_forward * wanderFwd);
}

Vector Steer::StayWithinMesh() const
{
	Vector origin = GetOrigin(m_pEntity);
	Vector center;
	
	MAKE_VECTORS(m_pEntity->v.angles);
	Vector dir = gpGlobals->v_forward;

	bool walkable = (g_pfnIsDirectionWalkable)(origin, dir, m_pEntity->v.maxs.x, 32, center);
	if (walkable)
	{
		//SERVER_PRINT(UTIL_VarArgs("oh inside %.f %.f %.f\n", center.x, center.y, center.z));
		return Vector();
	}

	//SERVER_PRINT(UTIL_VarArgs("oh outside %.f %.f %.f\n", center.x, center.y, center.z));
	return (center - origin).Normalize();
}

Vector Steer::Truncate(const Vector& vec, float maxLength) const
{
	float maxLengthSquared = maxLength * maxLength;
	float vecLengthSquared = DotProduct(vec, vec);

	if (vecLengthSquared <= maxLengthSquared)
		return vec;

	return vec * (maxLength / sqrt(vecLengthSquared));
}

Vector Steer::AvoidCloseMonsters(float minSeparationDistance) const
{
	Vector origin = GetOrigin(m_pEntity);
	MAKE_VECTORS(m_pEntity->v.angles);
	Vector forward = gpGlobals->v_forward;

	edict_t* pSearchEnt = NULL;
	while ((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, origin, 1000)) != NULL)
	{
		if (pSearchEnt == m_pEntity || FNullEnt(pSearchEnt))
			continue;

		if (pSearchEnt->v.solid == SOLID_SLIDEBOX && pSearchEnt->v.movetype == MOVETYPE_STEP)
		{
			float sumOfRadii = m_pEntity->v.maxs.x + pSearchEnt->v.maxs.x;
			float minCenterToCenter = minSeparationDistance + sumOfRadii;

			Vector offset = GetOrigin(pSearchEnt) - origin;
			float currentDistance = offset.Length();

			if (currentDistance < minCenterToCenter)
			{
				return PerpendicularComponent(-offset, forward);
			}
		}
	}

	return Vector();
}

float Steer::ComputeNearestApproachPositions(edict_t* pOther, float time, Vector &myFinal, Vector &otherFinal) const
{
	MAKE_VECTORS(m_pEntity->v.angles);
	Vector myTravel = gpGlobals->v_forward * m_pEntity->v.velocity.Length2D() * time;

	MAKE_VECTORS(pOther->v.angles);
	Vector otherTravel = gpGlobals->v_forward * pOther->v.velocity.Length2D() * time;

	myFinal = GetOrigin(m_pEntity) + myTravel;
	otherFinal = GetOrigin(pOther) + otherTravel;

	return (myFinal - otherFinal).Length();
}

float Steer::ScalarRandomWalk(float initial, float walkspeed, float min, float max) const
{
	float next = initial + (((UTIL_FRandom01() * 2) - 1) * walkspeed);
	if (next < min) return min;
	if (next > max) return max;
	return next;
}

Vector Steer::PerpendicularComponent(const Vector& vec, const Vector& unitBasis) const
{
	float projection = DotProduct(vec, unitBasis);
	return vec - unitBasis * projection;
}

Vector Steer::Flee(const Vector& target) const
{
	Vector force;
	Vector desired = GetOrigin(m_pEntity) - target;
	desired.Normalize();

	force = desired - m_pEntity->v.clbasevelocity;

	return force;
}

Vector Steer::Evade(edict_t* pEntity) const
{
	Vector distance = GetOrigin(pEntity) - GetOrigin(m_pEntity);

	float updates = distance.Length() / m_pEntity->v.maxspeed;

	Vector futurePos = GetOrigin(pEntity) + pEntity->v.velocity * updates;

	return Flee(futurePos);
}

Vector Steer::GetOrigin(edict_t* pEntity) const
{
	Vector orig = pEntity->v.origin;
	orig.z += pEntity->v.mins.z;

	return orig;
}

float Steer::PredictNearestApproachTime(edict_t* pOther) const
{
	Vector myVelocity = m_pEntity->v.velocity;
	Vector otherVelocity = pOther->v.velocity;
	Vector relVelocity = otherVelocity - myVelocity;
	float relSpeed = relVelocity.Length();

	if (relSpeed == 0) return 0;

	Vector relTangent = relVelocity / relSpeed;
	Vector relPosition = GetOrigin(m_pEntity) - GetOrigin(pOther);
	float projection = DotProduct(relTangent, relPosition);

	return projection / relSpeed;
}

Vector Steer::FollowPath(const Vector& sp, const Vector& ep, const std::vector<Vector>& path, int maxFollows) const
{
	if (path.size() > 2)
	{
		int num = min((int)path.size(), maxFollows);

		for (int i = 1; i < num; i++)
		{
			Vector a = path[i - 1];
			Vector b = path[i];
			a.z += 16;
			b.z += 16;

			Vector2D c;
			float fraction = DistPointSegment2D(sp.Make2D(), a.Make2D(), b.Make2D(), c);
			if (fraction < 1.0)
			{
				Vector d = b - a;
				d.Normalize();

				Vector e = b + d * 32;

				return Seek(e);
			}
		}
	}

	return Vector();
}

float Steer::DistPointSegment2D(const Vector2D& p, const Vector2D& sp0, const Vector2D& sp1, Vector2D& out) const
{
	Vector2D v = sp1 - sp0;
	Vector2D w = p - sp0;

	float c1 = DotProduct(w, v);
	if (c1 <= 0)
	{
		out = sp0;
		return 0.0;
	}

	float c2 = DotProduct(v, v);
	if (c2 <= c1)
	{
		out = sp1;
		return 0.0;
	}

	float d = c1 / c2;
	out = sp0 + d * v;

	return d / v.Length();
}