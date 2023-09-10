#pragma once

#include "Steer.h"

#include <vector>

#include "extdll.h"
#include "meta_api.h"

class MonsterBase
{
public:
	MonsterBase(edict_t* pEntity)
		: m_pEdict(pEntity), m_pEnemy(nullptr), m_pTarget(nullptr),
		m_activity(ACT_RESET), m_idealActivity(ACT_RESET), m_bloodColor(DONT_BLEED),
		m_conditions(0), m_memory(0)
	{
		m_steer.SetEntity(pEntity);
	}

	MonsterBase();

	virtual ~MonsterBase() {};

	virtual void Update() = 0;
	virtual void Spawn() = 0;

	void SetClassname(const char* pszClassname);

	void AddCondition(int condition) { m_conditions |= condition; }
	void PopCondition(int condition) { m_conditions &= ~condition; }
	bool HasCondition(int condition) { return (m_conditions & condition); }

	void AddMemory(int memory) { m_memory |= memory; }
	void PopMemory(int memory) { m_memory &= ~memory; }
	bool HasMemory(int memory) { return (m_memory & memory); }

	edict_t* m_pEdict;
	edict_t* m_pEnemy;
	edict_t* m_pTarget;
	Activity m_activity;
	Activity m_idealActivity;
	int m_conditions;
	int m_memory;
	int m_bloodColor;
	Steer m_steer;
};