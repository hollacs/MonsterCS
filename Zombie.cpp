#include "Zombie.h"
#include "NavmeshHeader.h"
#include "Utils.h"

#include "meta_api.h"

extern PFN_GET_CLOSEST_NAVMESH g_pfnGetClosestNavmesh;
extern PFN_FIND_SHORTEST_PATH g_pfnFindShortestPath;

/*
Zombie::Zombie(const Vector& origin, float health, float maxspeed, float gravity)
{
	edict_t* pEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	SET_MODEL(pEnt, "models/bio_test/zombi_1pve_man.mdl");
	SET_SIZE(pEnt, Vector(-16, -16, -36), Vector(16, 16, 36));
	SET_ORIGIN(pEnt, origin);

	pEnt->v.classname = MAKE_STRING("monster_test");
	pEnt->v.takedamage = DAMAGE_AIM;
	pEnt->v.health = health;
	pEnt->v.solid = SOLID_SLIDEBOX;
	pEnt->v.movetype = MOVETYPE_STEP;
	pEnt->v.deadflag = DEAD_NO;
	pEnt->v.maxspeed = maxspeed;
	pEnt->v.gamestate = 1;
	pEnt->v.enemy = NULL;
	pEnt->v.gravity = gravity;
	pEnt->v.friction = 1.0;

	pEnt->v.nextthink = gpGlobals->time + 0.1;

	m_pEdict = pEnt;
	m_steer.SetEntity(pEnt);
}

Zombie::Zombie(edict_t* pEntity)
{
	m_pEdict = pEntity;
	m_steer.SetEntity(pEntity);
}

void Wander(void)
{
	m_steer.Reset();
	m_steer.AddForce(m_steer.AvoidMonsters(3.0) * 8.0);
	m_steer.AddForce(m_steer.StayWithinMesh() * 5.0);
	m_steer.AddForce(m_steer.Wander(0.1));
	m_steer.Update();

	edict_t* pEnemy = NULL;

	edict_t* pSearchEnt = NULL;
	while ((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, m_pEdict->v.origin, 400)) != NULL)
	{
		if (strcmp(STRING(pSearchEnt->v.classname), "player") == 0 && pSearchEnt->v.deadflag == DEAD_NO)
		{
			if (UTIL_IsInViewCone(m_pEdict, pSearchEnt->v.origin) && UTIL_IsVisible(m_pEdict, pSearchEnt))
			{
				pEnemy = pSearchEnt;
			}
		}
	}

	if (pEnemy != NULL)
	{
		m_pEdict->v.enemy = pEnemy;
	}
}

void Zombie::Update()
{
	edict_t* pEntity = this->GetEdict();

	float walkspeed = pEntity->v.velocity.Length2D();
	if (walkspeed > 160)
		pEntity->v.sequence = 11;
	else if (walkspeed > 80)
		pEntity->v.sequence = 9;
	else if (walkspeed > 0)
		pEntity->v.sequence = 7;
	else
		pEntity->v.sequence = 1;

	pEntity->v.framerate = 1.0;
	pEntity->v.nextthink = gpGlobals->time + 0.1;
}

ZombieHandler::~ZombieHandler()
{
	this->Clear();
}

Zombie* ZombieHandler::Create(const Vector& origin, float health, float maxspeed, float gravity)
{
	Zombie* pZombie = new Zombie(origin, health, maxspeed, gravity);
	m_edicts[pZombie->GetEdict()] = pZombie;
	return pZombie;
}

void ZombieHandler::Remove(Zombie* pZombie)
{
	m_edicts.erase(pZombie->GetEdict());
	delete pZombie;
}

void ZombieHandler::Update(edict_t* pEntity) const
{
	Zombie* pZombie = GetZombie(pEntity);
	if (pZombie != nullptr)
	{
		pZombie->Update();
	}
}

void ZombieHandler::Clear()
{
	for (auto it = m_edicts.begin(); it != m_edicts.end(); it++)
	{
		Zombie *pZombie = it->second;
		delete pZombie;
	}

	m_edicts.clear();
}

Zombie* ZombieHandler::GetZombie(edict_t* pEntity) const
{
	auto search = m_edicts.find(pEntity);
	if (search != m_edicts.end())
	{
		Zombie* pZombie = search->second;
		return pZombie;
	}

	return nullptr;
}

void Zombie::Machine::Wander()
{
	m_pCurrent->Wander(this);
}

void Zombie::Machine::Chase()
{
	m_pCurrent->Chase(this);
}

void Zombie::WANDER::Chase(Machine* pMachine)
{
	pMachine->SetCurrent(new CHASE());
}
*/

Zombie::Zombie() : m_state(ZOMBIESTATE_NONE), m_animFinished(0), m_animDuration(0), m_nextAttack(0)
{
	SetClassname("monster_zombie");
}

void Zombie::Precache()
{
	// do something
}

void Zombie::Idle()
{
	m_pEdict->v.clbasevelocity = Vector();
	SetAnimation(ACT_IDLE);
}

void Zombie::Wander()
{
	m_steer.AddForce(m_steer.AvoidMonsters(3.0f) * 8.0f);
	m_steer.AddForce(m_steer.StayWithinMesh() * 2.0f);
	m_steer.AddForce(m_steer.Wander(0.1f));

	if (this->ShouldJump())
		this->Jump();
	else
		SetAnimation(ACT_WALK);
}

void Zombie::Alert(const Vector& threatPosition)
{
	m_lastThreatPos = threatPosition;
	m_steer.AddForce(m_steer.Seek(threatPosition, 99999999));
	SetAnimation(ACT_THREAT_DISPLAY);
}

void Zombie::MoveToThreat(const Vector& threatPosition)
{
	Vector src = m_pEdict->v.origin;
	src.z += m_pEdict->v.mins.z;

	Vector dest = threatPosition;
	dest.z -= 30;

	Navmesh* pSrc = (g_pfnGetClosestNavmesh)(src, 500);
	Navmesh* pDest = (g_pfnGetClosestNavmesh)(dest, 500);

	if (pSrc != nullptr && pDest != nullptr)
	{
		std::vector<Vector> path;
		if ((g_pfnFindShortestPath)(src, dest, pSrc, pDest, m_pEdict->v.maxs.x, 10, path))
		{
			m_pEdict->v.maxspeed = 90;
			m_steer.AddForce(m_steer.AvoidMonsters(3.0f) * 10.0);
			m_steer.AddForce(m_steer.FollowPath(src, dest, path, 10));

			if (this->ShouldJump())
				this->Jump();
			else
				SetAnimation(ACT_WALK_SCARED);
		}
	}
}

void Zombie::Chase(edict_t* pTarget)
{
	Vector src = m_pEdict->v.origin;
	src.z += m_pEdict->v.mins.z;

	Vector dest = pTarget->v.origin;
	dest.z += pTarget->v.mins.z;

	Navmesh* pSrc = (g_pfnGetClosestNavmesh)(src, 500);
	Navmesh* pDest = (g_pfnGetClosestNavmesh)(dest, 500);

	if (pSrc != nullptr && pDest != nullptr)
	{
		std::vector<Vector> path;
		if ((g_pfnFindShortestPath)(src, dest, pSrc, pDest, m_pEdict->v.maxs.x, 10, path))
		{
			m_pEdict->v.maxspeed = 220;
			m_steer.AddForce(m_steer.AvoidMonsters(3.0f) * 10.0f);

			Vector follow = m_steer.FollowPath(src, dest, path, 10);
			if (follow == Vector())
			{

			}

			m_steer.AddForce();

			if (this->ShouldJump())
				this->Jump();
			else
				SetAnimation(ACT_WALK);
		}
	}
}

void Zombie::MeleeAttack(edict_t* pVictim)
{
	SetAnimation(ACT_MELEE_ATTACK1);
	m_nextAttack = gpGlobals->time + 1.0;
}

edict_t *Zombie::CanMeleeAttack(edict_t* pTarget, float distance)
{
	if (gpGlobals->time < m_nextAttack)
		return NULL;

	Vector src, dest;
	src = m_pEdict->v.origin;
	src.z += 10;

	dest = src + (pTarget->v.origin - src).Normalize() * distance;

	TraceResult ptr;
	TRACE_HULL(src, dest, dont_ignore_monsters, head_hull, m_pEdict, &ptr);

	edict_t* pHit = ptr.pHit;
	if (!FNullEnt(pHit) && strcmp(STRING(pHit->v.classname), "player") == 0)
		return pHit;
	
	return NULL;
}

bool Zombie::ShouldJump()
{
	// cannot jump in air
	if (~m_pEdict->v.flags & FL_ONGROUND)
		return false;

	Vector src = m_pEdict->v.origin;
	src.z += m_pEdict->v.mins.z;
	src.z += 20;

	MAKE_VECTORS(m_pEdict->v.angles);

	Vector dest;
	dest = src + gpGlobals->v_forward * 32;

	// lower trace
	TraceResult ptr;
	TRACE_LINE(src, dest, ignore_monsters, m_pEdict, &ptr);

	float fraction = ptr.flFraction;
	if (fraction < 1.0) // obstructs by something
	{
		src.z += 30;
		dest = src + gpGlobals->v_forward * 32;

		// upper trace
		TRACE_LINE(src, dest, ignore_monsters, m_pEdict, &ptr);

		// upper trace has higher fraction than lower trace
		// which means there was something you can jump over
		if (ptr.flFraction > fraction)
		{
			// make src as end position of trace result
			src = ptr.vecEndPos;
			dest = src;
			dest.z -= 50; // move down position to 50 units

			TRACE_LINE(src, dest, ignore_monsters, m_pEdict, &ptr);
			
			// 50 - 18 = 32 units (18 is max height of stairs)
			// check if the upper floor is taller than a step of stair
			if ((src - ptr.vecEndPos).Length() <= 32)
			{
				// yes, we should jump now!
				return true;
			}
		}
	}

	// no, we don't have to jump now.
	return false;
}

void Zombie::Jump()
{
	m_pEdict->v.velocity.z = sqrt(2.0 * 800.0f * 50.0f);
	SetAnimation(ACT_HOP);
}

void Zombie::SetAnimation(Activity activity)
{
	float speed = m_pEdict->v.velocity.Length2D();

	switch (activity)
	{
		case ACT_IDLE:
		case ACT_WALK:
		case ACT_WALK_SCARED:
		{
			if ((m_activity != ACT_HOP || m_animFinished) &&
				(m_activity != ACT_LAND || m_animFinished) && 
				(m_activity != ACT_MELEE_ATTACK1 || m_animFinished))
			{
				// currect activity is not falling
				if (m_activity != ACT_FALL)
				{
					// on ground
					if (m_pEdict->v.flags & FL_ONGROUND)
					{
						// don't change
						m_idealActivity = activity;
					}
					// falling velocity is high
					else if (m_pEdict->v.velocity.z < -250)
					{
						// change it to falling
						m_idealActivity = ACT_FALL;
					}
				}
				else // activity is falling
				{
					// check if landing
					if (m_pEdict->v.velocity.z >= 0 || m_pEdict->v.flags & FL_ONGROUND)
					{
						m_idealActivity = ACT_LAND;
					}
					else
					{
						m_idealActivity = m_activity;
					}
				}
			}
			else
			{
				m_idealActivity = ACT_RESET;
			}
			break;
		}
		case ACT_THREAT_DISPLAY:
		{
			if (m_activity == activity)
				m_idealActivity = ACT_RESET;
			else
				m_idealActivity = activity;

			break;
		}
		default:
		{
			m_idealActivity = activity;
			break;
		}
	}

	switch (m_idealActivity)
	{
		case ACT_IDLE:
		{
			m_pEdict->v.sequence = 1;
			m_activity = m_idealActivity;
			m_animDuration = 0.0;
			break;
		}
		case ACT_WALK:
		{
			if (speed <= 135)
			{
				if (speed <= 80)
					m_pEdict->v.sequence = 7;
				else
					m_pEdict->v.sequence = 9;

				m_activity = ACT_WALK;
			}
			else
			{
				m_pEdict->v.sequence = 11;
				m_activity = ACT_RUN;
			}

			m_animDuration = 0.0;
			break;
		}
		case ACT_WALK_SCARED:
		{
			m_pEdict->v.sequence = 8;
			m_activity = m_idealActivity;
			m_animDuration = 0.0;
			break;
		}
		case ACT_HOP:
		{
			SERVER_PRINT("Jump...");
			UTIL_SetSequence(m_pEdict, 12, true);
			m_activity = m_idealActivity;
			m_animDuration = 1.0;
			m_animFinished = false;
			break;
		}
		case ACT_FALL:
		{
			UTIL_SetSequence(m_pEdict, 14, false);
			m_activity = m_idealActivity;
			m_animDuration = 0.0;
			break;
		}
		case ACT_LAND:
		{
			UTIL_SetSequence(m_pEdict, 15, false);

			m_animDuration = 0.75;
			m_animFinished = false;
			m_activity = m_idealActivity;
			break;
		}
		case ACT_MELEE_ATTACK1:
		{
			if (speed <= 135)
				UTIL_SetSequence(m_pEdict, RANDOM_LONG(0, 1) ? 32 : 34, true);
			else
				UTIL_SetSequence(m_pEdict, 35, true);

			m_activity = m_idealActivity;
			m_animDuration = 0.5;
			m_animFinished = false;
			break;
		}
		case ACT_THREAT_DISPLAY:
		{
			UTIL_SetSequence(m_pEdict, 22, false);
			m_activity = m_idealActivity;
			m_animDuration = 0.0;
			break;
		}
	}
}

edict_t* Zombie::FindEnemy(float radius, bool visible, bool inViewCone, bool nearest)
{
	float dist;
	float minDist = radius;

	edict_t* pEnemy = NULL;
	edict_t* pSearchEnt = NULL;

	while ((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, m_pEdict->v.origin, radius)) != NULL)
	{
		if (strcmp(STRING(pSearchEnt->v.classname), "player") == 0 && pSearchEnt->v.deadflag == DEAD_NO)
		{
			if (inViewCone && !UTIL_IsInViewCone(m_pEdict, pSearchEnt->v.origin))
				continue;

			if (visible && !UTIL_IsVisible(m_pEdict, pSearchEnt))
				continue;

			if (nearest)
			{
				dist = (m_pEdict->v.origin - pSearchEnt->v.origin).Length();
				if (dist > minDist)
					continue;

				minDist = dist;
			}

			pEnemy = pSearchEnt;
		}
	}

	return pEnemy;
}

void Zombie::Update()
{
	if (gpGlobals->time >= m_pEdict->v.fuser4)
	{
		m_steer.Reset();

		switch (m_state)
		{
			case ZOMBIESTATE_IDLE:
			{
				this->Idle();

				edict_t *pEnemy = FindEnemy(1750, false, false, false);
				if (pEnemy != NULL)
					m_state = ZOMBIESTATE_WANDER;

				break;
			}
			case ZOMBIESTATE_WANDER:
			{
				edict_t* pEnemy = FindEnemy(750, true, true, true);
				if (pEnemy != NULL)
				{
					m_state = ZOMBIESTATE_ALERT;
					m_lastAlertTime = gpGlobals->time;
					m_pEnemy = pEnemy;
				}
				else
				{
					this->Wander();
				}

				break;
			}
			case ZOMBIESTATE_ALERT:
			{
			}
			case ZOMBIESTATE_ATTACK:
			{
				if (!FNullEnt(m_pTarget) && m_pTarget->v.deadflag == DEAD_NO && UTIL_IsEntOnPVS(m_pTarget, m_pEdict))
				{
					if (this->CanMeleeAttack(m_pTarget, 45))
					{
						this->MeleeAttack(m_pTarget);
					}
					else
					{
						this->Chase(m_pTarget);
					}
				}
				else
				{
					m_pEnemy = NULL;
					m_pTarget = NULL;
					m_state = ZOMBIESTATE_IDLE;
				}
			}
		}

		m_steer.Update();

		if (!m_animFinished && m_animDuration > 0.0f)
		{
			if (gpGlobals->time >= m_pEdict->v.animtime + m_animDuration)
				m_animFinished = true;
		}

		m_pEdict->v.fuser4 = gpGlobals->time + 0.1f;
	}

	if (m_pEdict->v.velocity.x < 0.0001f && m_pEdict->v.velocity.x >= 0.0f)
		m_pEdict->v.velocity.x = 0.0001f;
	else if (m_pEdict->v.velocity.x > -0.0001f && m_pEdict->v.velocity.x < 0.0f)
		m_pEdict->v.velocity.x = -0.0001f;

	m_pEdict->v.nextthink = gpGlobals->time;
}

void Zombie::Spawn()
{
	SET_MODEL(m_pEdict, "models/bio_test/zombi_1pve_man.mdl");
	SET_SIZE(m_pEdict, Vector(-16, -16, -36), Vector(16, 16, 36));

	m_pEdict->v.takedamage = DAMAGE_AIM;
	m_pEdict->v.health = 250;
	m_pEdict->v.solid = SOLID_SLIDEBOX;
	m_pEdict->v.movetype = MOVETYPE_STEP;
	m_pEdict->v.deadflag = DEAD_NO;
	m_pEdict->v.maxspeed = 220;
	m_pEdict->v.gravity = 1.0;
	m_pEdict->v.friction = 1.0;
	m_pEdict->v.view_ofs = VEC_VIEW;
	m_pEdict->v.fov = 80;

	m_pEdict->v.frame = 0;
	m_pEdict->v.framerate = 1.0;
	m_pEdict->v.animtime = gpGlobals->time;

	m_state = ZOMBIESTATE_IDLE;

	m_bloodColor = BLOOD_COLOR_RED;

	m_pEdict->v.nextthink = gpGlobals->time + 0.1;
}
