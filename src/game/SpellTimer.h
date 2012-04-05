#ifndef SPELLTIMER_H
#define SPELLTIMER_H

#include "Unit.h"
#include <Util.h>

enum UnitSelectType
{
    UNIT_SELECT_NONE = 0,       // no select, used if no need for unit or if unit is set manually
    UNIT_SELECT_SELF,           // select unit itself
    UNIT_SELECT_VICTIM,         // select unit's target
    UNIT_SELECT_GUID,           // select by specific guid set in targetInfo
    UNIT_SELECT_RANDOM_PLAYER,  // select random player, ignoring targetInfo cout of threat table
    UNIT_SELECT_RANDOM_UNIT     // select random unit, ignoring targetInfo cout of threat table
};

enum CastType
{
    CAST_TYPE_NONCAST = 0,      // if there is currently any spell casted, do nothing
    CAST_TYPE_QUEUE,            // if there is currently any spell casted, put cast into queue
    CAST_TYPE_FORCE,            // if there is currently any spell casted, interrupt that spell a cast this one instead
    CAST_TYPE_IGNORE            // dont check cast
};

enum TimerValues
{
    TIMER_VALUE_ALL             =-1,
    TIMER_VALUE_COOLDOWN        = 0,
    TIMER_VALUE_SPELLID         = 1,
    TIMER_VALUE_TIMER           = 2,
    TIMER_VALUE_UPDATEABLE      = 3,
    TIMER_VALUE_DELETE_AT_FINISH= 4,
    TIMER_VALUE_JUST_FINISHED   = 5,
    TIMER_VALUE_CUSTOM          = 6
};

struct RV
{
    uint32 uMin;
    uint32 uMax;
    uint32 uExact;
    explicit RV(uint32 _min, uint32 _max) : uMin(_min), uMax(_max), uExact(0) {}
    RV(uint32 _exact = 0) : uMin(0), uMax(0), uExact(_exact) {}
    RV(const RV &_rv) : uMin(_rv.uMin), uMax(_rv.uMax), uExact(_rv.uExact) {}

    bool isEmpty() const
    {
        return uMin == 0 && uMax == 0 && uExact == 0;
    }

    bool isExact() const
    {
        return uExact != 0;
    }

    uint32 GetVal()
    {
        if (uExact)
            return uExact;
        else if (uMax < uMin)
            return urand(uMax, uMin);
        return urand(uMin, uMax);
    }
};

struct SpellTimer
{
    SpellTimer(uint32 initialSpellId, RV initialTimer, RV initialCooldown, UnitSelectType targetType = UNIT_SELECT_NONE, CastType castType = CAST_TYPE_NONCAST, uint64 targetInfo = NULL, Unit* caster = NULL);

    public:
        void CheckInitialCooldown();

        void Reset(TimerValues value);
        void SetValue(TimerValues value, uint64 newValue);
        uint32 GetValue(TimerValues value);

        void Cooldown(RV cd = NULL, bool permanent = false);
        void SetTarget(Unit* target) { target_m = target; }

        bool IsReady();
        bool IsCastable();

        void Update(uint32 diff);

        bool Finish(Unit* target = NULL);

        Unit* getCaster()        const { return caster_m; }
        Unit* getTarget()        const { return target_m; }
        Unit* findTarget(Unit* target = NULL);

        uint32 getGCD();
        CastType getCastType()   const { return castType_m; }
        bool  isCasterCasting()  const { return caster_m && caster_m->IsNonMeleeSpellCasted(false); }

    private:
        uint32 timer_m;
        RV cooldown_m;
        uint32 spellId_m;

        uint32 initialSpellId_m;
        RV initialTimer_m;
        RV initialCooldown_m;

        UnitSelectType targetType_m;
        CastType castType_m;

        uint64 targetInfo_m;
        Unit* caster_m;
        Unit* target_m;

        bool updateAllowed_m;
        bool shouldDeleteWhenFinish_m;
        bool justFinished_m;
        uint64 customValue_m;
};

#endif // SPELLTIMER_H
