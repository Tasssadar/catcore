#include "Unit.h"
#include "SpellTimer.h"
#include "SpellTimerMgr.h"

SpellTimerMgr::SpellTimerMgr(Unit* unit) : m_owner(unit)
{
    ASSERT(m_owner != NULL);
    m_GCD = new SpellTimer(GCD_ID, 0, 0);
}
SpellTimerMgr::~SpellTimerMgr()
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        delete itr->second;

    m_TimerMap.clear();

    delete m_GCD;
}

void SpellTimerMgr::AddTimer(uint32 timerId, uint32 initialSpellId, uint32 initialTimer, int32 initialCooldown, UnitSelectType targetType, CastType castType, uint64 targetInfo, Unit* caster)
{
    // is already set, delete
    if (m_TimerMap[timerId])
        RemoveTimer(timerId);

    m_TimerMap[timerId] = new SpellTimer(initialSpellId, initialTimer, initialCooldown, targetType, castType, targetInfo, caster ? caster : m_owner);
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
    m_GCD->Update(uiDiff);

    if (m_TimerMap.empty())
        return;

    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        if (itr->second->isUpdateable())
            itr->second->Update(uiDiff);

    if (m_IdToBeCasted.empty())
        return;

    for(TimerIdList::iterator itr = m_IdToBeCasted.begin(); itr != m_IdToBeCasted.end(); ++itr)
    {
        if (CanBeTimerFinished(*itr))
        {
            FinishTimer(*itr);
            m_IdToBeCasted.erase(itr);
        }
    }
}

bool SpellTimerMgr::IsReady(uint32 timerId)
{
    return m_TimerMap[timerId]->IsReady();
}

uint32 SpellTimerMgr::GetSpellId(uint32 timerId)
{
    return m_TimerMap[timerId]->getSpellId();
}

void SpellTimerMgr::Cooldown(uint32 timerId, uint32 changedCD, bool permanent)
{
    m_TimerMap[timerId]->Cooldown(changedCD, permanent);
}

bool SpellTimerMgr::CheckTimer(uint32 timerId, Unit *target)
{
    SpellTimer* timer = m_TimerMap[timerId];

    // timer with timerId not found
    if (!timer)
        return false;

    //  if timer isn't ready
    if (!timer->IsReady())
        return false;

    // custom handeling differed by cast type
    if (timer->getCastType() == CAST_TYPE_NONCAST)
    {
        if (!CanBeTimerFinished(timerId))
            return false;
    }
    else if (timer->getCastType() == CAST_TYPE_QUEUE)
    {
        if (!CanBeTimerFinished(timerId))
        {
            timer->SetTarget(target);
            m_IdToBeCasted.push_back(timerId);
            return false;
        }
    }

    return FinishTimer(timerId,target);
}

bool SpellTimerMgr::CanBeTimerFinished(uint32 timerId)
{
    if (m_TimerMap[timerId]->getCaster() != m_owner)
        return true;

    if (m_owner->IsNonMeleeSpellCasted(false))
        return false;

    if (!m_GCD->IsReady())
        return false;

    return true;
}

bool SpellTimerMgr::FinishTimer(uint32 timerId, Unit *target)
{
    if (m_TimerMap[timerId]->Finish(target))
    {
        m_GCD->Cooldown(m_TimerMap[timerId]->getGCD());
        return true;
    }

    return false;
}
