#pragma once

#include "extdll.h"

void UTIL_ClientPrintAll(int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4);

char* UTIL_VarArgs(const char* format, ...);

edict_t* UTIL_FindEntityByClassname(edict_t* pStartEntity, const char* szName);

edict_t* UTIL_FindEntityByString(edict_t* pentStart, const char* szKeyword, const char* szValue);

edict_t* UTIL_FindEntityInSphere(edict_t* pStart, const Vector& vecCenter, float flRadius);

void UTIL_SetSequence(edict_t* pEntity, int sequence, bool reset=false);

bool UTIL_IsInViewCone(edict_t* pEntity, const Vector& origin);

bool UTIL_IsVisible(edict_t* pEntity, edict_t* pTarget);

bool UTIL_IsVisible(edict_t* pEntity, const Vector& pos);

bool UTIL_CanHear(edict_t* pEntity, const Vector& pos, float sensitivity, float volume);

float UTIL_FRandom01();

bool UTIL_IsEntOnPVS(edict_t* pPlayer, edict_t* pTarget);