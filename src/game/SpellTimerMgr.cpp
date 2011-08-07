#include "CreatureCastMgr.h"
#include "Unit.h"
#include "SpellTimer.h"

CreatureCastMgr::CreatureCastMgr()
{
}

CreatureCastMgr::~CreatureCastMgr()
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        delete itr->second;

    m_TimerMap.clear();
}

void CreatureCastMgr::AddTimer(uint32 timerId, Unit* caster, uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType, CastType castType, uint64 targetInfo)
{
    // is already set, delete
    if (m_TimerMap[timerId])
        RemoveTimer(timerId);

    m_TimerMap[timerId] = new SpellTimer(caster, initialSpellId, initialTimer, initialCooldown, targetType, castType, targetInfo);
}

void CreatureCastMgr::RemoveTimer(uint32 timerId)
{
    delete m_TimerMap[timerId];
    m_TimerMap.erase(timerId);
}

SpellTimer* CreatureCastMgr::GetTimer(uint32 timerId)
{
    return m_TimerMap[timerId];
}

void CreatureCastMgr::UpdateTimers(const uint32 uiDiff)
{
    if (m_TimerMap.empty())
        return;

    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        if (itr->second->isUpdateable())
            itr->second->Update(uiDiff);
}

bool CreatureCastMgr::IsReady(uint32 timerId)
{
    return m_TimerMap[timerId]->IsReady();
}

uint32 CreatureCastMgr::GetSpellId(uint32 timerId)
{
    return m_TimerMap[timerId]->GetSpellId();
}

void CreatureCastMgr::Cooldown(uint32 timerId)
{
    m_TimerMap[timerId]->Cooldown();
}

bool CreatureCastMgr::CheckTimer(uint32 timerId, Unit *target)
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
