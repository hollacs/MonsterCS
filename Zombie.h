#pragma once

#include "MonsterBase.h"
#include "extdll.h"

typedef enum
{

	ZOMBIESTATE_NONE = 0,
	ZOMBIESTATE_IDLE,
	ZOMBIESTATE_WANDER,
	ZOMBIESTATE_ALERT,
	ZOMBIESTATE_ATTACK,
	ZOMBIESTATE_EVADE,
	ZOMBIESTATE_DEAD,

} ZOMBIESTATE;

class Zombie : public MonsterBase
{
public:
	Zombie(edict_t* pEntity)
		: MonsterBase(pEntity), m_state(ZOMBIESTATE_NONE), m_animFinished(0), m_animDuration(0), m_nextAttack(0)
	{};
	Zombie();

	~Zombie() {};

	void Precache();

	void Idle();
	void Wander();
	void Alert(const Vector& threatPosition);
	void MoveToThreat(const Vector& threatPosition);
	void Chase(edict_t* pTarget);

	edict_t* CanMeleeAttack(edict_t* pTarget, float distance);
	void MeleeAttack(edict_t* pVictim);

	bool ShouldJump();
	void Jump();

	void SetAnimation(Activity activity);

	edict_t* FindEnemy(float dist, bool visible, bool inViewCone, bool nearest);

	void Update() override;
	void Spawn() override;

private:
	bool m_animFinished;
	float m_animDuration;
	float m_nextAttack;
	float m_lastAlertTime;
	ZOMBIESTATE m_state;
	Vector m_lastThreatPos;
};