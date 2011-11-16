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
    Clear();
    delete m_GCD;
}

void SpellTimerMgr::AddTimer(uint32 timerId, uint32 initialSpellId, RV initialTimer, RV initialCooldown, UnitSelectType targetType, CastType castType, uint64 targetInfo, Unit* caster)
{
    if (!timerId || timerId >= QUEUE_TIMER_ID)
    {
        sLog->outError("SpellTimerMgr:: Could not add timer id %i for spell %u, because limit for timer id is %u, but seriously, do you need that number to be so high ?!?!", timerId, initialSpellId, uint32(QUEUE_TIMER_ID));
        return;
    }

    // is already set, delete
    if (m_TimerMap[timerId])
        RemoveTimer(timerId);

    m_TimerMap[timerId] = new SpellTimer(initialSpellId, initialTimer, initialCooldown, targetType, castType, targetInfo, caster ? caster : m_owner);
}

void SpellTimerMgr::AddSpellToQueue(uint32 spellId, UnitSelectType targetType, uint64 targetInfo)
{
    // target must be set right away
    if (targetType == UNIT_SELECT_NONE)
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

    SpellTimer* timer = new SpellTimer(spellId, 0, 0, targetType, CAST_TYPE_QUEUE, targetInfo, m_owner);
    timer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);

    m_TimerMap[timerId] = timer;
    m_IdToBeCasted.push_back(timerId);
}

bool SpellTimerMgr::AddSpellCast(uint32 spellId, UnitSelectType targetType, uint64 targetInfo)
{
    // target must be set right away
    if (targetType == UNIT_SELECT_NONE)
        return false;

    SpellTimer tempTimer(spellId, 0, 0, targetType, CAST_TYPE_FORCE, targetInfo, m_owner);
    return FinishTimer(&tempTimer);
}

void SpellTimerMgr::RemoveTimer(uint32 timerId)
{
    SpellTimerMap::iterator itr = m_TimerMap.find(timerId);
    if (itr != m_TimerMap.end())
    {
        delete itr->second;
        m_TimerMap.erase(itr);
    }
}

void SpellTimerMgr::UpdateTimers(const uint32 uiDiff)
{
    if (!isUpdatable)
        return;

    m_GCD->Update(uiDiff);

    if (m_TimerMap.empty())
        return;

    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
    {
        if (SpellTimer* timer = itr->second)
        {
            if (timer->JustFinished())
            {
                if (timer->ShouldBeDeleted())
                {
                    RemoveTimer(itr->first);
                    continue;
                }

                timer->SetTarget(NULL);
                timer->SetValue(TIMER_VALUE_JUST_FINISHED, false);
            }
            if (timer->IsUpdatable())
                timer->Update(uiDiff);
        }
        else
        {
            sLog->outError("SpellTimerMgr:: Update: In TimerMap of unit with entry %u was found invalid SpellTimer, its iterator will be erased", m_owner->GetEntry());
            m_TimerMap.erase(itr);
            continue;
        }
    }

    if (!m_IdToBeCasted.empty())
    {
        for(TimerIdList::iterator itr = m_IdToBeCasted.begin(), next; itr != m_IdToBeCasted.end(); itr = next)
        {
            next = itr;
            ++next;
            if (SpellTimer* timer = GetTimer(*itr))
                if (!CanFinishTimer(timer) || !FinishTimer(timer))
                    continue;
            m_IdToBeCasted.erase(itr);
        }
    }
}

SpellTimer* SpellTimerMgr::GetTimer(uint32 timerId)
{
    if (!timerId)
        return NULL;

    SpellTimerMap::iterator itr = m_TimerMap.find(timerId);
    if (itr == m_TimerMap.end())
        return NULL;

    if (!itr->second)
    {
        sLog->outError("SpellTimerMgr:: GetTimer: Invalid iterator found on key %u, erasing it from TimerMap ...", timerId);
        m_TimerMap.erase(itr);
        return NULL;
    }

    return itr->second;
}

bool SpellTimerMgr::IsReady(uint32 timerId)
{
    if (SpellTimer* timer = GetTimer(timerId))
        return timer->IsReady();

    return false;
}

void SpellTimerMgr::Reset(uint32 timerId, TimerValues value)
{
    if (SpellTimer* timer = GetTimer(timerId))
        timer->Reset(value);
}

void SpellTimerMgr::SetValue(uint32 timerId, TimerValues value, uint64 newValue)
{
    if (SpellTimer* timer = GetTimer(timerId))
        timer->SetValue(value, newValue);
}

uint32 SpellTimerMgr::GetValue(uint32 timerId, TimerValues value)
{
    if (SpellTimer* timer = GetTimer(timerId))
        return timer->GetValue(value);

    return 0;
}

void SpellTimerMgr::Cooldown(uint32 timerId, RV changedCD, bool permanent)
{
    if (SpellTimer* timer = GetTimer(timerId))
        timer->Cooldown(changedCD, permanent);
}

bool SpellTimerMgr::CanFinishTimer(SpellTimer* timer)
{
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

SpellTimer* SpellTimerMgr::GetState(uint32 timerId, Unit *target)
{
    SpellTimer* timer = GetTimer(timerId);

    // timer with timerId not found
    if (!timer)
        return NULL;

    //  if timer isn't ready
    if (!timer->IsReady())
        return NULL;

    // custom handeling differed by cast type
    switch(timer->getCastType())
    {
        case CAST_TYPE_NONCAST:
        case CAST_TYPE_QUEUE:
        {
            if (!CanFinishTimer(timer))
            {
                if (timer->getCastType() == CAST_TYPE_QUEUE)
                {
                    timer->SetTarget(target);
                    m_IdToBeCasted.push_back(timerId);
                }
                return NULL;
            }
            break;
        }
        case CAST_TYPE_FORCE:
        case CAST_TYPE_IGNORE:
            break;
        default:
            break;
    }

    timer->SetTarget(target);

    if (!FinishTimer(timer))
        return NULL;

    return timer;
}

bool SpellTimerMgr::FinishTimer(SpellTimer* timer, Unit *target)
{
    if (target)
        timer->SetTarget(target);

    if (timer->Finish())
    {
        m_GCD->Cooldown(timer->getGCD());
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
        if (!timer)
        {
            sLog->outError("SpellTimerMgr:: ProhibitSpellSchool: Wrong iterator found while trying to prohibit spells, erasing it from map ...");
            m_TimerMap.erase(itr);
            continue;
        }

        SpellEntry const* spellInfo = timer->GetTimerSpellEntry();
        if (!spellInfo)
            continue;

        if ((idSchoolMask & GetSpellSchoolMask(spellInfo)) &&  timer->GetRemainingDuration() < unTimeMs)
            timer->Cooldown(unTimeMs);
    }
}

void SpellTimerMgr::Clear()
{
    for(SpellTimerMap::iterator itr = m_TimerMap.begin(); itr != m_TimerMap.end(); ++itr)
        delete itr->second;

    m_TimerMap.clear();
}
