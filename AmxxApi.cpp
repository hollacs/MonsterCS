#include "AmxxApi.h"
#include "amxxmodule.h"
#include "HLTypeConversion.h"
#include "MonsterManager.h"
#include "Utils.h"

#define CHECK_ENTITY(x) \
	if (x < 0 || x > gpGlobals->maxEntities) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Entity out of range (%d)", x); \
		return 0; \
	} else { \
		if (x > 0 && x <= gpGlobals->maxClients) { \
			if (!MF_IsPlayerIngame(x)) { \
				MF_LogError(amx, AMX_ERR_NATIVE, "Invalid player %d (not in-game)", x); \
				return 0; \
			} \
		} else { \
			if (x != 0 && FNullEnt(TypeConversion.id_to_edict(x))) { \
				MF_LogError(amx, AMX_ERR_NATIVE, "Invalid entity %d", x); \
				return 0; \
			} \
		} \
	}

extern HLTypeConversion TypeConversion;

/*
static cell AMX_NATIVE_CALL zombie_set_heard(AMX * amx, cell * params)
{
	int iEnt = params[1];

	CHECK_ENTITY(iEnt);

	cell* vAmx = MF_GetAmxAddr(amx, params[2]);
	REAL fX = amx_ctof(vAmx[0]);
	REAL fY = amx_ctof(vAmx[1]);
	REAL fZ = amx_ctof(vAmx[2]);

	REAL fDist = amx_ctof(params[3]);
	REAL fVolume = amx_ctof(params[4]);

	Vector pos = Vector(fX, fY, fZ);

	edict_t* pEnt = TypeConversion.id_to_edict(iEnt);

	MonsterBase *pMonster = g_monsterManager.GetMonster(pEnt);
	Zombie

	if (UTIL_CanHear(pEnt, pos, fDist, fVolume))
	{
		
	}

	return 0;
}


static cell AMX_NATIVE_CALL can_monster_hear(AMX* amx, cell* params)
{
	int iEnt = params[1];

	cell* vAmx = MF_GetAmxAddr(amx, params[2]);
	REAL fX = amx_ctof(vAmx[0]);
	REAL fY = amx_ctof(vAmx[1]);
	REAL fZ = amx_ctof(vAmx[2]);

	REAL fDist = amx_ctof(params[3]);
	REAL fVolume = amx_ctof(params[4]);

	Vector pos = Vector(fX, fY, fZ);

	CHECK_ENTITY(iEnt);

	edict_t* pEnt = TypeConversion.id_to_edict(iEnt);

	float distance = fDist * fVolume;
	if (!UTIL_IsVisible(pEnt, pos))
		distance *= 0.5;

	Vector earPos = pEnt->v.origin + pEnt->v.view_ofs;

	if ((earPos - pos).Length() <= distance)
		return 1;

	return 0;
}
*/

void RegisterNatives()
{

}

AMX_NATIVE_INFO monster_Natives[] = {
	{"",		},

	{NULL,					NULL}
	///////////////////
};