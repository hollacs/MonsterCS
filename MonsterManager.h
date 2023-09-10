#pragma once

#include "MonsterBase.h"

#include <map>

class MonsterManager
{
public:
	MonsterManager() {};
	~MonsterManager();

	void Register(MonsterBase *pMonster);
	void Remove(MonsterBase* pMonster);
	void Update(edict_t* pEntity) const;
	void Clear();
	MonsterBase* GetMonster(edict_t* pEntity) const;

private:
	std::map<edict_t*, MonsterBase*> m_edicts;
};