#include "SpellTimer.h"
#include "DBCStores.h"
#include "Map.h"

SpellTimer::SpellTimer(uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType, CastType castType, uint64 targetInfo, Unit* caster) :
    initialSpellId_m(initialSpellId), initialTimer_m(initialTimer), targetType_m(targetType), castType_m(castType), targetInfo_m(targetInfo), caster_m(caster), target_m(NULL)
{
    SetInitialCooldown(initialCooldown);
    Reset(TIMER_VALUE_ALL);
}

void SpellTimer::Reset(TimerValues value)
{
    switch (value)
    {
        case TIMER_VALUE_ALL:
            cooldown_m = initialCooldown_m;
            spellId_m = initialSpellId_m;
            timer_m = initialTimer_m;
            updateAllowed_m = true;
            break;
        case TIMER_VALUE_COOLDOWN:
            cooldown_m = initialCooldown_m;
            break;
        case TIMER_VALUE_SPELLID:
            spellId_m = initialSpellId_m;
            break;
        case TIMER_VALUE_TIMER:
            timer_m = initialTimer_m;
            break;
        case TIMER_VALUE_UPDATEABLE:
            updateAllowed_m = true;
        default:
            break;
    }
}

void SpellTimer::SetValue(TimerValues value, uint32 newValue)
{
    switch (value)
    {
        case TIMER_VALUE_COOLDOWN:
            cooldown_m = newValue;
            break;
        case TIMER_VALUE_SPELLID:
            spellId_m = newValue;
            break;
        case TIMER_VALUE_TIMER:
            timer_m = newValue;
            break;
        case TIMER_VALUE_UPDATEABLE:
            updateAllowed_m = bool(newValue);
            break;
        default:
            break;
    }
}

void SpellTimer::SetInitialCooldown(int32 cooldown)
{
    if (cooldown <= DBC_COOLDOWN)
    {
        SpellEntry const *spellInfo = sSpellStore.LookupEntry(initialSpellId_m);
        initialCooldown_m = spellInfo ? spellInfo->RecoveryTime : 0;
    }
    else
        initialCooldown_m = cooldown;
}

void SpellTimer::Update(uint32 diff)
{
    if (timer_m < diff)
        timer_m = 0;
    else
        timer_m -= diff;
}

bool SpellTimer::IsReady()
{
    return timer_m == 0;
}

uint32 SpellTimer::getGCD()
{
    if (SpellEntry const *spellInfo = sSpellStore.LookupEntry(initialSpellId_m))
        return spellInfo->StartRecoveryTime;

    return NULL;
}

Unit* SpellTimer::getTarget(Unit* target)
{
    if (!caster_m || !caster_m->IsInWorld())
        return NULL;

    if (target)
        target_m = target;

    if (target_m)
        return target_m;

    switch(targetType_m)
    {
        case UNIT_SELECT_SELF:      SetTarget(caster_m); break;
        case UNIT_SELECT_VICTIM:    SetTarget(caster_m->getVictim()); break;
        case UNIT_SELECT_GUID:      SetTarget(caster_m->GetMap()->GetUnit(ObjectGuid(targetInfo_m))); break;
        case UNIT_SELECT_RANDOM_PLAYER:
            if (caster_m->GetTypeId() == TYPEID_UNIT)
                SetTarget(((Creature*)caster_m)->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, targetInfo_m));
            break;
        case UNIT_SELECT_RANDOM_UNIT:
            if (caster_m->GetTypeId() == TYPEID_UNIT)
                SetTarget(((Creature*)caster_m)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, targetInfo_m));
            break;
        default:
            break;
    }

    if (!target_m)
    {
        // here could be used specific targets types for finding right target from implicitTargets
        SpellEntry const *spellInfo = sSpellStore.LookupEntry(initialSpellId_m);
        if (spellInfo)
        {
            if (sSpellMgr.IsSelfOnlyCast(spellInfo))
                SetTarget(caster_m);
        }
    }

    return target_m;
}

void SpellTimer::Cooldown(uint32 cd, bool permanent)
{
    if (cd && permanent)
        cooldown_m = cd;

    timer_m = cd ? cd : cooldown_m;
}

bool SpellTimer::Finish(Unit *target)
{
    // if timer for not existing spell, dont even try to finish it
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(initialSpellId_m);
    if (!spellInfo)
        return false;

    Unit* uCaster = getCaster();
    Unit* uTarget = getTarget(target);

    // checking just target, caster checked in getTarget()
    if (!uTarget || !uTarget->IsInWorld())
        return false;

    if (castType_m == CAST_TYPE_FORCE)
        uCaster->InterruptNonMeleeSpells(false);

    uCaster->CastSpell(uTarget, spellInfo, false);
    Cooldown();
    SetTarget(NULL);
    return true;
}

// for case of useage outside of SpellTimerMgr
bool SpellTimer::IsCastable()
{
    if (!IsReady())
        return false;

    if (!caster_m)
        return false;

    if (castType_m != CAST_TYPE_FORCE && caster_m->IsNonMeleeSpellCasted(false))
        return false;

    return true;
}
