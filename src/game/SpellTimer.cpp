#include "SpellTimer.h"
#include "DBCStores.h"

void SpellTimer::Update(uint32 diff)
{
    if (currentTimer < diff)
        currentTimer = 0;
    else
        currentTimer -= diff;
}

void SpellTimer::SetInitialCooldown(int32 cooldown)
{
    if (cooldown == DBC_COOLDOWN)
    {
        SpellEntry const* spellEntry = sSpellStore.LookupEntry(GetSpellId());
        if (spellEntry)
            cooldown_m = spellEntry->RecoveryTime;
        else
            cooldown_m = 0;
    }
    else if (cooldown < 0)
    {
        cooldown_m = 0;
        return;
    }
    else
        cooldown_m = cooldown;
}

bool SpellTimer::CheckAndUpdate(uint32 diff, bool isCreatureCurrentlyCasting)
{
    if (currentTimer < diff)
    {
        currentTimer = 0;
        if (check_cast_m && isCreatureCurrentlyCasting)
            return false;

        return true;
    }
    else
    {
        currentTimer -= diff;
        return false;
    }
}

void SpellTimerMgr::Add(uint32 name, uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast)
{
    SpellTimer* timer = new SpellTimer(initialSpellId, initialTimer, cooldown, check_cast);
    m_TimerMap[name] = timer;
}

void SpellTimerMgr::Remove(uint32 name)
{
    delete m_TimerMap[name];
}

void SpellTimerMgr::Update(const uint32 uiDiff)
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        if (SpellTimer* timer = itr->second)
            timer->Update(uiDiff);
}

