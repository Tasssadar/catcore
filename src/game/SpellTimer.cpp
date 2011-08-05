#include "SpellTimer.h"
#include "DBCStores.h"

void SpellTimer::Update(uint32 diff)
{
    if (timer < diff)
        timer = 0;
    else
        timer -= diff;
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
    if (timer < diff)
    {
        timer = 0;
        if (check_cast_m && isCreatureCurrentlyCasting)
            return false;

        return true;
    }
    else
    {
        timer -= diff;
        return false;
    }
}

bool SpellTimer::IsReady(bool isCreatureCurrentlyCasting)
{
    if (check_cast_m && isCreatureCurrentlyCasting)
        return false;

    return timer == 0;
}

SpellTimerMgr::~SpellTimer()
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        delete itr->second;

    m_TimerMap.clear();
}

void SpellTimerMgr::Add(uint32 name, uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast, bool auto_updateable)
{
    // is already set, delete
    if (m_TimerMap[name])
        delete m_TimerMap[name];

    SpellTimer* timer = new SpellTimer(initialSpellId, initialTimer, cooldown, check_cast, auto_updateable);
    m_TimerMap[name] = timer;
}

void SpellTimerMgr::Remove(uint32 name)
{
    delete m_TimerMap[name];
}

void SpellTimerMgr::Get(uint32 name)
{
    return m_TimerMap[name];
}

void SpellTimerMgr::Update(const uint32 uiDiff)
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        if (SpellTimer* timer = itr->second)
            if (timer->isUpdateable())
                timer->Update(uiDiff);
}

bool SpellTimerMgr::IsReady(uint32 name, bool isCreatureCurrentlyCasting) const
{
    return m_TimerMap[name] && m_TimerMap[name]->IsReady(isCreatureCurrentlyCasting);
}

uint32 SpellTimerMgr::GetSpellId(uint32 name) const
{
    return m_TimerMap[name] ? m_TimerMap[name]->GetSpellId() : 0;
}

void SpellTimerMgr::AddCooldown(uint32 name)
{
    if (m_TimerMap[name])
        m_TimerMap[name]->AddCooldown();
}

