#ifndef SPELLTIMER_H
#define SPELLTIMER_H

#define DBC_COOLDOWN -1

struct SpellTimer
{
    SpellTimer(uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast) :
            initialSpellId_m(initialSpellId), initialTimer_m(initialTimer), check_cast_m(check_cast)
    {
        SetInitialCooldown(cooldown);
        ResetTimerToInitialValue();
        ResetSpellId();
    }

    public:
        void SetInitialCooldown(int32 cooldown);
        void ResetTimerToInitialValue() { currentTimer = initialTimer_m; }
        void ResetSpellId() { spellId_m = initialSpellId_m; }

        void SetTimer(uint32 newTimer) { currentTimer = newTimer; }
        void ChangeSpellId(uint32 newId) { spellId_m = newId; }

        void Update(uint32 diff);
        void AddCooldown() { currentTimer = cooldown_m; }

        bool CheckAndUpdate(uint32 diff, bool is_casting);
        bool IsReady() const { return currentTimer == 0; }
        uint32 GetSpellId() const { return spellId_m; }

    private:
        uint32  currentTimer;

        uint32  spellId_m;
        uint32  cooldown_m;

        uint32  initialTimer_m;
        uint32  initialSpellId_m;

        bool    check_cast_m;
};

typedef std::map<uint32, SpellTimer*> SpellTimerMap;

struct SpellTimerMgr
{
    SpellTimerMgr() : {}

    public:
        void Add(uint32 name, uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast = true);
        void Remove(uint32 name);
        void Update(uint32 const uiDiff);

    private:
        SpellTimerMap m_TimerMap;

};

#endif // SPELLTIMER_H
