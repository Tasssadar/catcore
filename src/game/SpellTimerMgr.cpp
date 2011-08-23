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
    if (timerId >= QUEUE_TIMER_ID)
    {
        sLog.outError("SpellTimerMgr:: Could not add timer id %i for spell %u, because limit for timer id is %u, but seriously, do you need that number to be so high ?!?!", timerId, initialSpellId, uint32(QUEUE_TIMER_ID));
        return;
    }

    // is already set, delete
    if (m_TimerMap[timerId])
        RemoveTimer(timerId);

    m_TimerMap[timerId] = new SpellTimer(initialSpellId, initialTimer, initialCooldown, targetType, castType, targetInfo, caster ? caster : m_owner);
}

void SpellTimerMgr::AddSpellToQueue(uint32 spellId, UnitSelectType targetType, uint64 targetInfo, Unit *target)
{
    // target must be set right away
    if (targetType == UNIT_SELECT_NONE && !target)
        return;

    uint32 timerId = 0;
    if (m_IdToBeCasted.empty())
        timerId = QUEUE_TIMER_ID;
    else
    {
        SpellTimerMap::iterator itr = m_TimerMap.end();
        --itr;
        timerId = itr->first + 1;
    }

    ASSERT(timerId != 0);

    SpellTimer* timer = new SpellTimer(spellId, 0, 0, targetType, CAST_TYPE_QUEUE, targetInfo);
    m_TimerMap[timerId] = timer;

    if (target)
        timer->SetTarget(target);

    timer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);

    m_IdToBeCasted.push_back(timerId);
}

void SpellTimerMgr::RemoveTimer(uint32 timerId)
{
    delete m_TimerMap[timerId];
    m_TimerMap.erase(timerId);
}

void SpellTimerMgr::UpdateTimers(const uint32 uiDiff)
{
    if (!isUpdatable)
        return;

    m_GCD->Update(uiDiff);

    if (!m_TimerMap.empty())
    {
        for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        {
            if (SpellTimer* timer = itr->second)
            {
                if (timer->GetValue(TIMER_VALUE_UPDATEABLE))
                    timer->Update(uiDiff);
            }
            else
            {
                uint32 timerId = itr->first;
                sLog.outCatLog("SpellTimerMgr:: Update: Timer Manager  for creature %u has wrong timer id %s known (%u), deleting it ...", m_owner->GetEntry(), timerId ? "IS" : "ISNOT", timerId);
                m_TimerMap.erase(itr);
            }

        }

        if (!m_IdToBeCasted.empty())
        {
            for(TimerIdList::iterator itr = m_IdToBeCasted.begin(); itr != m_IdToBeCasted.end(); ++itr)
            {
                if (uint32 timerId = *itr)
                {
                    if (m_TimerMap[timerId])
                    {
                        if (CanBeTimerFinished(timerId))
                            if (FinishTimer(timerId))
                                m_IdToBeCasted.erase(itr);
                    }
                    else
                    {
                        sLog.outCatLog("SpellTimerMgr:: QueueUpdate: Queue accesing for non-existing timer in TimerMgr of creature %u, wrong timerId is %u, spell is beeing deleted from queue...", m_owner->GetEntry(), timerId);
                        m_IdToBeCasted.erase(itr);
                    }
                }
                else
                {
                    sLog.outCatLog("SpellTimerMgr:: QueueUpdate: In cast queue in TimerMgr of creature %u save non-existing id, deleteing iterator", m_owner->GetEntry());
                    m_IdToBeCasted.erase(itr);
                }
            }
        }
    }
}

bool SpellTimerMgr::IsReady(uint32 timerId)
{
    if (SpellTimer* timer = m_TimerMap[timerId])
        return timer->IsReady();

    return false;
}

SpellTimer* SpellTimerMgr::GetTimer(uint32 timerId)
{
    return m_TimerMap[timerId];
}

void SpellTimerMgr::Reset(uint32 timerId, TimerValues value)
{
    if (SpellTimer* timer = m_TimerMap[timerId])
        timer->Reset(value);
}

void SpellTimerMgr::SetValue(uint32 timerId, TimerValues value, uint32 newValue)
{
    if (SpellTimer* timer = m_TimerMap[timerId])
        timer->SetValue(value, newValue);
}

uint32 SpellTimerMgr::GetValue(uint32 timerId, TimerValues value)
{
    if (SpellTimer* timer = m_TimerMap[timerId])
        return timer->GetValue(value);

    return 0;
}

void SpellTimerMgr::Cooldown(uint32 timerId, uint32 changedCD, bool permanent)
{
    if (SpellTimer* timer = m_TimerMap[timerId])
        timer->Cooldown(changedCD, permanent);
}

bool SpellTimerMgr::CanBeTimerFinished(uint32 timerId)
{
    SpellTimer* timer = m_TimerMap[timerId];
    if (!timer)
        return false;

    if (timer->getCaster() != m_owner)
        return true;

    if (m_owner->IsNonMeleeSpellCasted(false))
        return false;

    if (!m_GCD->IsReady())
        return false;

    return true;
}

bool SpellTimerMgr::TimerFinished(uint32 timerId, Unit *target)
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

    timer->SetTarget(target);
    return FinishTimer(timerId);
}

bool SpellTimerMgr::FinishTimer(uint32 timerId)
{
    SpellTimer* timer = m_TimerMap[timerId];

    if (timer->Finish())
    {
        m_GCD->Cooldown(timer->getGCD());
        if (timer->GetValue(TIMER_VALUE_DELETE_AT_FINISH))
            RemoveTimer(timerId);
        return true;
    }

    return false;
}

void SpellTimerMgr::ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs)
{
    // no timers
    if (m_TimerMap.empty())
        return;

    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
    {
        SpellTimer* timer = itr->second;
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(timer->GetValue(TIMER_VALUE_SPELLID));

        if ((idSchoolMask & GetSpellSchoolMask(spellInfo)) &&  timer->GetValue(TIMER_VALUE_TIMER) < unTimeMs)
            timer->Cooldown(unTimeMs);
    }
}
