#include "Utils.h"

#include "meta_api.h"

void UTIL_ClientPrintAll(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4)
{
	static int msgTextMsg = 0;

	if (msgTextMsg == 0)
	{
		msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL);

		if (!msgTextMsg)
			return;
	}

	MESSAGE_BEGIN(MSG_ALL, msgTextMsg);
	WRITE_BYTE(msg_dest);
	WRITE_STRING(msg_name);
	if (param1)
		WRITE_STRING(param1);
	if (param2)
		WRITE_STRING(param2);
	if (param3)
		WRITE_STRING(param3);
	if (param4)
		WRITE_STRING(param4);
	MESSAGE_END();
}

char* UTIL_VarArgs(const char* format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

edict_t* UTIL_FindEntityByClassname(edict_t* pStartEntity, const char* szName)
{
	return UTIL_FindEntityByString(pStartEntity, "classname", szName);
}

edict_t* UTIL_FindEntityByString(edict_t* pentStart, const char* szKeyword, const char* szValue)
{
	edict_t* pentEntity;

	pentEntity = FIND_ENTITY_BY_STRING(pentStart, szKeyword, szValue);

	if (!FNullEnt(pentEntity))
		return pentEntity;

	return NULL;
}

edict_t* UTIL_FindEntityInSphere(edict_t* pStart, const Vector& vecCenter, float flRadius)
{
	if (!pStart) pStart = NULL;

	pStart = FIND_ENTITY_IN_SPHERE(pStart, vecCenter, flRadius);

	if (!FNullEnt(pStart)) return pStart;
	return NULL;
}

void UTIL_SetSequence(edict_t* pEntity, int sequence, bool reset)
{
	if (pEntity->v.sequence == sequence)
		return;

	pEntity->v.sequence = sequence;

	if (reset)
	{
		pEntity->v.frame = 0;
		pEntity->v.animtime = gpGlobals->time;
		pEntity->v.framerate = 1.0;
	}
}

bool UTIL_CanHear(edict_t* pEntity, const Vector& pos, float sensitivity, float volume)
{
	float distance = sensitivity * volume;
	if (!UTIL_IsVisible(pEntity, pos))
		distance *= 0.5;

	Vector earPos = pEntity->v.origin + pEntity->v.view_ofs;

	if ((earPos - pos).Length() <= distance)
		return 1;

	return 0;
}

bool UTIL_IsVisible(edict_t* pEntity, const Vector& pos)
{
	TraceResult ptr;
	TRACE_LINE(pEntity->v.origin + pEntity->v.view_ofs, pos, ignore_monsters, pEntity, &ptr);

	if (ptr.flFraction == 1.0)
	{
		return true;
	}

	return false;
}

bool UTIL_IsVisible(edict_t* pEntity, edict_t* pTarget)
{
	TraceResult ptr;
	TRACE_LINE(pEntity->v.origin + pEntity->v.view_ofs, pTarget->v.origin, dont_ignore_monsters, pEntity, &ptr);

	if (ptr.flFraction < 1.0)
	{
		if (ptr.pHit == pTarget)
		{
			return true;
		}
	}

	return false;
}

bool UTIL_IsInViewCone(edict_t* pEntity, const Vector& origin)
{
	Vector vecLOS, vecForward;
	float flDot;

	MAKE_VECTORS(pEntity->v.angles);
	vecForward = gpGlobals->v_forward;
	vecLOS = origin - (pEntity->v.origin + pEntity->v.view_ofs);

	vecLOS = vecLOS.Normalize();
	flDot = DotProduct(vecLOS, vecForward);

	if (flDot >= cos(pEntity->v.fov * (M_PI / 360)))
		return true;

	return false;
}

float UTIL_FRandom01()
{
	return RANDOM_FLOAT(0.0, 1.0);
}

bool UTIL_IsEntOnPVS(edict_t* pPlayer, edict_t* pTarget)
{
	edict_t *pNext = (*g_engfuncs.pfnEntitiesInPVS)(pPlayer);

	while (pNext != NULL)
	{
		if (pNext == pTarget)
			return true;

		edict_t *pChain = pNext->v.chain;
		if (pChain == NULL)
			break;

		pNext = pChain;
	}

	return false;
}