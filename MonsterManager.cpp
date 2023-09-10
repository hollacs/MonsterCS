#include "MonsterManager.h"

MonsterManager::~MonsterManager()
{
	this->Clear();
}

void MonsterManager::Register(MonsterBase* pMonster)
{
	m_edicts[pMonster->m_pEdict] = pMonster;
}

void MonsterManager::Remove(MonsterBase* pMonster)
{
	m_edicts.erase(pMonster->m_pEdict);
	delete pMonster;
}

void MonsterManager::Update(edict_t* pEntity) const
{
	MonsterBase* pMonster = GetMonster(pEntity);
	if (pMonster != nullptr)
	{
		pMonster->Update();
	}
}

void MonsterManager::Clear()
{
	for (auto it = m_edicts.begin(); it != m_edicts.end(); it++)
	{
		MonsterBase* pMonster = it->second;
		delete pMonster;
	}

	m_edicts.clear();
}

MonsterBase* MonsterManager::GetMonster(edict_t* pEntity) const
{
	auto search = m_edicts.find(pEntity);
	if (search != m_edicts.end())
	{
		MonsterBase* pMonster = search->second;
		return pMonster;
	}

	return nullptr;
}
