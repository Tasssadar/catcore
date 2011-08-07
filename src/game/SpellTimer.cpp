#include "SpellTimer.h"
#include "DBCStores.h"
#include "Map.h"

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
    if (cooldown == DBC_COOLDOWN)
    {
        SpellEntry const* spellEntry = sSpellStore.LookupEntry(GetSpellId());
        if (spellEntry)
            initialCooldown_m = spellEntry->RecoveryTime;
        else
            initialCooldown_m = 0;
    }
    else if (cooldown < 0)
    {
        initialCooldown_m = 0;
        return;
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
    if (castType_m == CAST_TYPE_NONCAST && isCasterCasting())
        return false;

    return timer_m == 0;
}

bool SpellTimer::Finish(Unit* target)
{
    target_m = target ? target : getTarget();
    if (!target_m)
        return false;

    caster_m->CastSpell(target, GetSpellId(), false);
    Cooldown();
    target_m = NULL;
    return true;
}

Unit* SpellTimer::getTarget()
{
    if (!caster_m || !caster_m->IsInWorld())
        return NULL;

    switch(targetType_m)
    {
        case UNIT_SELECT_SELF:      return caster_m;
        case UNIT_SELECT_VICTIM:    return caster_m->getVictim();
        case UNIT_SELECT_GUID:      return caster_m->GetMap()->GetUnit(ObjectGuid(targetInfo_m));
        case UNIT_SELECT_RANDOM_PLAYER:
            if (caster_m->GetTypeId() == TYPEID_UNIT)
                return ((Creature*)caster_m)->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, targetInfo_m);
        case UNIT_SELECT_RANDOM_UNIT:
            if (caster_m->GetTypeId() == TYPEID_UNIT)
                return ((Creature*)caster_m)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, targetInfo_m);
        default:
            break;
    }

    return target;
}

void SpellTimer::InterruptCastedSpells()
{
    if (caster_m)
        caster_m->InterruptNonMeleeSpells(false);
}
