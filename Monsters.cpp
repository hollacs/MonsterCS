#include "MonsterBase.h"

#include "meta_api.h"

MonsterBase::MonsterBase()
	: m_pEdict(nullptr), m_pTarget(nullptr), m_pEnemy(nullptr),
	m_activity(ACT_RESET), m_idealActivity(ACT_RESET), m_bloodColor(DONT_BLEED), 
	m_conditions(0), m_memory(0)
{
	edict_t* pEnt = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	pEnt->v.classname = MAKE_STRING("monster");
	pEnt->v.gamestate = 1;

	m_pEdict = pEnt;
	m_steer.SetEntity(pEnt);
}

void MonsterBase::SetClassname(const char* pszClassname)
{
	m_pEdict->v.classname = MAKE_STRING(pszClassname);
}