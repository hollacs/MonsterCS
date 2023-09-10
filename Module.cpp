#include "amxxmodule.h"
#include "Cmd.h"
#include "Utils.h"
#include "MonsterManager.h"
#include "Zombie.h"
#include "NavmeshHeader.h"
#include "AmxxApi.h"
#include "HLTypeConversion.h"
#include <set>

#define SIZE_OF(a) (sizeof(a)/sizeof(*a))

//const char *g_szAttackSound[] = { "zombie/claw_strike1.wav", "zombie/claw_strike2.wav", "zombie/claw_strike3.wav" };

PFN_GET_CLOSEST_NAVMESH g_pfnGetClosestNavmesh;
PFN_FIND_SHORTEST_PATH g_pfnFindShortestPath;
PFN_IS_DIRECTION_WALKABLE g_pfnIsDirectionWalkable;
HLTypeConversion TypeConversion;

void HandleCmd_Npc(edict_t* pEntity, const std::vector<std::string>& arguments);
static Cmd cmd_nav("npc", HandleCmd_Npc);

MonsterManager g_monsterManager;
AMX *g_pAmxScript;

/*
void SetLockSequence(edict_t* pEntity, int sequence, float locktime)
{
	UTIL_SetSequence(pEntity, sequence);

	pEntity->v.weaponanim = sequence;
	pEntity->v.scale = locktime;
}

void SetEntSequence(edict_t* pEntity, int sequence, bool reset=false, bool bypass=false)
{
	if (pEntity->v.sequence == pEntity->v.weaponanim && gpGlobals->time < pEntity->v.animtime + pEntity->v.scale && !bypass)
		return;
	
	if (pEntity->v.sequence == sequence && !reset)
		return;

	UTIL_SetSequence(pEntity, sequence);
}

edict_t* CreateZombieDirector(float time)
{
	edict_t* pEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	if (pEnt != NULL)
	{
		pEnt->v.classname = MAKE_STRING("zombie_director");
		pEnt->v.maxspeed = time;
		pEnt->v.nextthink = gpGlobals->time;
	}

	return pEnt;
}

edict_t* CreateMonster(const Vector& origin, const char* pszModel, const Vector& min, const Vector& max, const char* pszClassname)
{
	edict_t* pEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	if (pEnt != NULL)
	{
		SET_MODEL(pEnt, pszModel);
		SET_SIZE(pEnt, min, max);
		SET_ORIGIN(pEnt, origin);
		
		pEnt->v.modelindex = PRECACHE_MODEL(pszModel);
		pEnt->v.classname = MAKE_STRING(pszClassname);
		pEnt->v.takedamage = DAMAGE_AIM;
		pEnt->v.health = 300;
		pEnt->v.solid = SOLID_BBOX;
		pEnt->v.movetype = MOVETYPE_STEP;
		pEnt->v.deadflag = DEAD_NO;
		pEnt->v.maxspeed = 220;
		pEnt->v.gamestate = 1;
		pEnt->v.enemy = NULL;
		//pEnt->v.flags |= FL_MONSTER;
		pEnt->v.gravity = 1.0;
		pEnt->v.friction = 1.0;

		pEnt->v.nextthink = gpGlobals->time;
		g_zombieCount++;
	}

	return pEnt;
}

edict_t* FindClosestEnemy(edict_t* pEntity, float maxDistance, bool visible = false)
{
	float minDistance = maxDistance;
	edict_t* pResult = NULL;
	edict_t* pSearchEnt = NULL;
	while ((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, pEntity->v.origin, maxDistance)) != NULL)
	{
		// Search for alive player
		if (strcmp(STRING(pSearchEnt->v.classname), "player") == 0 && pSearchEnt->v.deadflag == DEAD_NO)
		{
			float distance = (pEntity->v.origin - pSearchEnt->v.origin).Length();
			if (distance < minDistance)
			{
				if (visible)
				{
					TraceResult ptr;
					TRACE_LINE(pEntity->v.origin, pSearchEnt->v.origin, ignore_monsters, pEntity, &ptr);

					if (ptr.flFraction < 1.0)
						continue;
				}

				minDistance = distance;
				pResult = pSearchEnt;
			}
		}
	}

	return pResult;
}*/

void OnPluginsLoaded()
{
	PRECACHE_MODEL("models/bio_test/zombi_1pve_man.mdl");

	TypeConversion.init();

	g_pfnGetClosestNavmesh = (PFN_GET_CLOSEST_NAVMESH)MF_RequestFunction("GetClosestNavmesh");
	g_pfnFindShortestPath = (PFN_FIND_SHORTEST_PATH)MF_RequestFunction("FindShortestPath");
	g_pfnIsDirectionWalkable = (PFN_IS_DIRECTION_WALKABLE)MF_RequestFunction("IsDirectionWalkable");

	RegisterNatives();
}

void OnPluginsUnloaded()
{
	g_monsterManager.Clear();
}

void ClientCommand(edict_t* pEntity)
{
	HandleCmds(pEntity);
}

void HandleCmd_Npc(edict_t* pEntity, const std::vector<std::string>& arguments)
{
	if (arguments.size() < 1)
		return;

	if (arguments[0] == "create")
	{
		Vector origin = pEntity->v.origin;
		origin.z += 100;

		Zombie* pZombie = new Zombie();
		g_monsterManager.Register(pZombie);
		pZombie->Spawn();

		edict_t* pEnt = pZombie->m_pEdict;

		pEnt->v.body = RANDOM_LONG(0, 6); // random submodel (cso)
		pEnt->v.skin = RANDOM_LONG(0, 1); // random skin (cso)
		pEntity->v.maxspeed = 600;

		SET_ORIGIN(pEnt, origin);
	}
}

/*
void ThinkMonster(edict_t* pEntity)
{
	// per 0.1 second to update
	if (gpGlobals->time >= pEntity->v.fuser4 + 0.1)
	{
		if (!pEntity->v.bInDuck && pEntity->v.deadflag == DEAD_DEAD)
		{
			g_zombieCount--;
			pEntity->v.bInDuck = true;
		}


		
		// check if the monster is alive
		if (pEntity->v.iuser1 == 0 && pEntity->v.deadflag == DEAD_NO)
		{
			edict_t* pEnemy = pEntity->v.enemy;

			// Enemy is not vaild
			if (FNullEnt(pEnemy) || pEnemy->v.deadflag != DEAD_NO)
			{
				// Find closest visible enemy
				pEntity->v.enemy = FindClosestEnemy(pEntity, 2048, true);
				pEnemy = pEntity->v.enemy;

				pEntity->v.teleport_time = gpGlobals->time + 15.0; // Set lock target time
			}

			// Enemy found
			if (!FNullEnt(pEnemy))
			{
				// Check if lock time has expired
				float lastLockTime = pEntity->v.teleport_time;
				if (gpGlobals->time >= lastLockTime)
				{
					// Find new target
					pEntity->v.enemy = FindClosestEnemy(pEntity, 4096, false);
					pEnemy = pEntity->v.enemy;

					pEntity->v.teleport_time = gpGlobals->time + 7.5; // Set new lock target time
				}

				// Enemy is still valid
				if (pEnemy != NULL)
				{
					// Check if monster can attack
					if ((~pEntity->v.button & IN_ATTACK) && gpGlobals->time >= pEntity->v.radsuit_finished)
					{
						Vector src, dest;
						src = pEntity->v.origin;
						src.z += 10;

						dest = src + (pEnemy->v.origin - pEntity->v.origin).Normalize() * 48;

						TraceResult ptr;
						TRACE_HULL(src, dest, dont_ignore_monsters, head_hull, pEntity, &ptr);

						edict_t* pHit = ptr.pHit;

						// if pHit is player
						if (!FNullEnt(pHit) && strcmp(STRING(pHit->v.classname), "player") == 0)
						{
							int victim = ENTINDEX(pHit);
							int attacker = ENTINDEX(pEntity);

							// a little hack for using hamsandwich's function
							MF_ExecuteForward(g_pfwTakeDamage, (cell)victim, (cell)attacker, (cell)attacker, 10.0f, 4);

							pEntity->v.enemy = pHit; // Change target
							pEnemy = pHit;

							pEntity->v.button |= IN_ATTACK;
							pEntity->v.button |= IN_ALT1;

							// running attack animation
							if (pEntity->v.velocity.Length2D() >= 150)
								SetLockSequence(pEntity, 35, 0.5);
							else // normal attack
								SetLockSequence(pEntity, RANDOM_LONG(32, 34), 0.5);

							g_engfuncs.pfnEmitSound(pEntity, CHAN_WEAPON, RANDOM_SOUND_ARRAY(g_szAttackSound), VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(95, 105));

							pEntity->v.radsuit_finished = gpGlobals->time + 2.0; // set next attack time
							pEntity->v.pain_finished = gpGlobals->time + 0.75;
						}
					}

					Vector src, dest;
					src = pEntity->v.origin;
					dest = pEnemy->v.origin;

					Vector steering;
					if ((src - dest).Length() <= 50) // if monster and enemy is close enough
					{
						// directly follow the enemy
						steering = (g_pfnSteerSeek)(pEntity, dest);
					}
					else
					{
						// tune down a little bit the origin
						src.z -= 30;
						dest.z -= 30;

						// get closest navmeshes
						Navmesh* pStart = (g_pfnGetClosestNavmesh)(src, 500, false, false);
						Navmesh* pGoal = (g_pfnGetClosestNavmesh)(dest, 500, false, false);

						// check if start and goal navmesh is valid
						if (pStart != nullptr && pGoal != nullptr)
						{
							// AStar pathfinding
							std::vector<Vector> path;
							if ((g_pfnFindShortestPath)(src, dest, pStart, pGoal, 10, path))
							{
								// Steering behavior
								steering = (g_pfnSteerAvoidMonsters)(pEntity, 3) * 10; // Add avoid monsters force
								steering = steering + (g_pfnSteerFollowPath)(pEntity, src, dest, path, 10) * 0.5; // Add path following force
							}
						}
					}

					Vector oldVelocity = pEntity->v.velocity;

					// Apply steering force for this monster
					(g_pfnSteerApplyForce)(pEntity, steering, 45, 1);

					Vector newVelocity = pEntity->v.velocity;

					if ((pEntity->v.flags & FL_ONGROUND) || !FNullEnt(pEntity->v.groundentity))
					{
						src = pEntity->v.origin;
						src.z -= 10;

						Vector v = newVelocity.Normalize();
						dest = src + v * (pEntity->v.maxspeed * 0.3);

						TraceResult ptr;
						TRACE_HULL(src, dest, dont_ignore_monsters, head_hull, pEntity, &ptr);

						float fraction = ptr.flFraction;
						if (fraction < 1.0) // stair
						{
							if (oldVelocity.Length() < 30)
							{
								if (pEntity->v.iuser2 == 0)
								{
									pEntity->v.iuser2 = 1;
									pEntity->v.fuser2 = gpGlobals->time;
								}
								else if (gpGlobals->time > pEntity->v.fuser2 + 3.0)
								{
									pEntity->v.iuser2 = 2;
								}
							}

							src = pEntity->v.origin;
							dest = src + v * (pEntity->v.maxspeed * 0.3);

							TRACE_HULL(src, dest, dont_ignore_monsters, head_hull, pEntity, &ptr);

							// normal
							if ((ptr.flFraction <= fraction && ptr.flFraction < 1.0 && ptr.pHit != pEnemy) || pEntity->v.iuser2 == 2)
							{
								fraction = ptr.flFraction;

								src = pEntity->v.origin;
								src.z += 32;
								dest = src + v * (pEntity->v.maxspeed * 0.3);

								TRACE_HULL(src, dest, dont_ignore_monsters, head_hull, pEntity, &ptr);

								if (ptr.flFraction > fraction)
								{
									pEntity->v.velocity.z = sqrt(2.0 * 800.0f * 50.0f);
									pEntity->v.iuser2 = 0;
									pEntity->v.button |= IN_JUMP;
									SetLockSequence(pEntity, 12, 0.75);
								}
							}
						}
						else
						{
							pEntity->v.iuser2 = 0;
						}
					}
				}
			}

			// check if attack time has expired
			if (pEntity->v.button & IN_ATTACK)
			{
				if (gpGlobals->time >= pEntity->v.radsuit_finished)
				{
					pEntity->v.button &= ~IN_ATTACK;
				}
			}
			if (pEntity->v.button & IN_ALT1)
			{
				if (gpGlobals->time >= pEntity->v.pain_finished)
				{
					pEntity->v.button &= ~IN_ALT1;
				}
				else
				{
					float z = pEntity->v.velocity.z;
					pEntity->v.velocity = pEntity->v.velocity * 0.3;
					pEntity->v.velocity.z = z;
				}
			}

			// monster animation management
			if ((pEntity->v.flags & FL_ONGROUND) || !FNullEnt(pEntity->v.groundentity))
			{
				if ((pEntity->v.oldbuttons & IN_JUMP) && (pEntity->v.button & IN_JUMP))
				{
					pEntity->v.button &= ~IN_JUMP;
				}

				if (pEntity->v.sequence == 14)
				{
					SetLockSequence(pEntity, 15, 0.5);
				}
				else
				{
					float walkspeed = pEntity->v.velocity.Length2D();
					if (walkspeed >= 190)
					{
						SetEntSequence(pEntity, 11);
					}
					else if (walkspeed >= 130)
					{
						SetEntSequence(pEntity, 9);
					}
					else if (walkspeed > 1)
					{
						SetEntSequence(pEntity, 7);
					}
					else
					{
						SetEntSequence(pEntity, 1);
					}
				}
			}
			else if (pEntity->v.velocity.z < -250)
			{
				pEntity->v.iuser2 = 0;
				SetEntSequence(pEntity, 14, false, true);
			}
		}

		pEntity->v.fuser4 = gpGlobals->time;
	}

	// Fix zero velocity animation bug
	if (pEntity->v.velocity.Length() < 10)
	{
		if (pEntity->v.velocity.x >= 0)
			pEntity->v.velocity.x = max(pEntity->v.velocity.x, 0.001);
		else if (pEntity->v.velocity.x < 0)
			pEntity->v.velocity.x = min(pEntity->v.velocity.x, -0.001);
	}

	pEntity->v.oldbuttons = pEntity->v.button;
	pEntity->v.nextthink = gpGlobals->time;
}
*/


void DispatchThink(edict_t* pEntity)
{
	g_monsterManager.Update(pEntity);
	RETURN_META(MRES_IGNORED);
}