#ifndef CREATURECASTMGR_H
#define CREATURECASTMGR_H

#include "SpellTimer.h"
#include "Unit.h"

typedef std::map<uint32, SpellTimer*> SpellTimerMap;
typedef std::list<uint32> TimerIdList;

#define GCD_ID 61304

struct SpellTimerMgr
{
    public:
        SpellTimerMgr(Unit* unit);
        ~SpellTimerMgr();

        void AddTimer(uint32 timerId, uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType = UNIT_SELECT_NONE, CastType castType = CAST_TYPE_NONCAST, uint64 targetInfo = 0, Unit* caster = NULL);
        void RemoveTimer(uint32 timerId);               // not safe to use right now
        SpellTimer* GetTimer(uint32 timerId);

        void UpdateTimers(uint32 const uiDiff);
        bool CheckTimer(uint32 timerId, Unit* target = NULL);
        void AddToCastQueue(uint32 timerId) { m_IdToBeCasted.push_back(timerId); }

        void Cooldown(uint32 timerId, uint32 changedCD = NULL, bool permanent = false);
        bool IsReady(uint32 timerId);
        uint32 GetSpellId(uint32 timerId);

        bool CanBeTimerFinished(uint32 timerId);
        bool FinishTimer(uint32 timerId);

        void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs);

    private:
        SpellTimerMap m_TimerMap;
        TimerIdList m_IdToBeCasted;

        SpellTimer* m_GCD;

        Unit* m_owner;
};

#endif // CREATURECASTMGR_H
