#ifndef CREATURECASTMGR_H
#define CREATURECASTMGR_H

#include "SpellTimer.h"
#include "Unit.h"

typedef std::map<uint32, SpellTimer*> SpellTimerMap;
typedef std::list<SpellTimer*> SpellTimerList;

struct CreatureCastMgr
{
    public:
        CreatureCastMgr();
        ~CreatureCastMgr();

        void AddTimer(uint32 timerId, Unit* caster, uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType = UNIT_SELECT_NONE, CastType castType = CAST_TYPE_NONCAST, uint64 targetInfo = 0);
        void RemoveTimer(uint32 timerId);               // not safe to use right now
        SpellTimer* GetTimer(uint32 timerId);

        void UpdateTimers(uint32 const uiDiff);
        bool CheckTimer(uint32 timerId, Unit* target = NULL);

        bool IsReady(uint32 timerId);
        uint32 GetSpellId(uint32 timerId);
        void Cooldown(uint32 timerId);

    private:
        SpellTimerMap m_TimerMap;
        SpellTimerList m_CastList;
};

#endif // CREATURECASTMGR_H
