#include "Unit.h"
#include "SpellTimer.h"
#include "SpellTimerMgr.h"

SpellTimerMgr::SpellTimerMgr()
{
}

SpellTimerMgr::~SpellTimerMgr()
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        delete itr->second;

    m_TimerMap.clear();
}

void SpellTimerMgr::AddTimer(uint32 timerId, Unit* caster, uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType, CastType castType, uint64 targetInfo)
{
    // is already set, delete
    if (m_TimerMap[timerId])
        RemoveTimer(timerId);

    m_TimerMap[timerId] = new SpellTimer(caster, initialSpellId, initialTimer, initialCooldown, targetType, castType, targetInfo);
}

void SpellTimerMgr::RemoveTimer(uint32 timerId)
{
    delete m_TimerMap[timerId];
    m_TimerMap.erase(timerId);
}

SpellTimer* SpellTimerMgr::GetTimer(uint32 timerId)
{
    return m_TimerMap[timerId];
}

void SpellTimerMgr::UpdateTimers(const uint32 uiDiff)
{
    if (m_TimerMap.empty())
        return;

    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        if (itr->second->isUpdateable())
            itr->second->Update(uiDiff);

    if (m_CastList.empty())
        return;

    for(SpellTimerList::iterator itr = m_CastList.begin(); itr != m_CastList.end(); ++itr)
    {
        SpellTimer* timer = *itr;
        if (timer && !timer->isCasterCasting())
        {
            timer->Finish();
            m_CastList.erase(itr);
        }
    }
}

bool SpellTimerMgr::IsReady(uint32 timerId)
{
    return m_TimerMap[timerId]->IsReady();
}

uint32 SpellTimerMgr::GetSpellId(uint32 timerId)
{
    return m_TimerMap[timerId]->GetSpellId();
}

void SpellTimerMgr::Cooldown(uint32 timerId)
{
    m_TimerMap[timerId]->Cooldown();
}

bool SpellTimerMgr::CheckTimer(uint32 timerId, Unit *target)
{
    // lets find timer
    SpellTimer* timer = m_TimerMap[timerId];

    //  if timer is ready
    if (!timer->IsReady())
        return false;

    // custom handeling differed by cast type
    if (timer->GetCastType() == CAST_TYPE_FORCE)
    {
        timer->InterruptCastedSpells();
        return timer->Finish(target);
    }
    else if (timer->GetCastType() == CAST_TYPE_QUEUE && timer->isCasterCasting())
    {
        m_CastList.push_back(timer);
        return false;
    }

    return timer->Finish(target);
}
