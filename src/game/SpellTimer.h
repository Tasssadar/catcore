#ifndef SPELLTIMER_H
#define SPELLTIMER_H

#define DBC_COOLDOWN -1

struct SpellTimer
{
    SpellTimer(uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast, bool auto_updateable) :
        initialSpellId_m(initialSpellId), initialTimer_m(initialTimer), check_cast_m(check_cast), auto_updateable_m(auto_updateable)
    {
        SetInitialCooldown(cooldown);
        ResetTimerToInitialValue();
        ResetSpellId();
    }

    public:
        void SetInitialCooldown(int32 cooldown);
        void ResetTimerToInitialValue() { timer = initialTimer_m; }
        void ResetSpellId() { spellId_m = initialSpellId_m; }

        void SetTimer(uint32 newTimer) { timer = newTimer; }
        void ChangeSpellId(uint32 newId) { spellId_m = newId; }

        void AddCooldown() { timer = cooldown_m; }

        bool CheckAndUpdate(uint32 diff, bool is_casting);
        bool IsReady(bool isCreatureCurrentlyCasting);
        void Update(uint32 diff);

        uint32 GetSpellId() const { return spellId_m; }
        bool isUpdateable() const { return auto_updateable_m; }

    private:
        uint32  timer;

        uint32  initialTimer_m;
        uint32  cooldown_m;

        uint32  spellId_m;
        uint32  initialSpellId_m;

        bool    check_cast_m;
        bool    auto_updateable_m;
};

typedef std::map<uint32, SpellTimer*> SpellTimerMap;

struct SpellTimerMgr
{
    SpellTimerMgr() {}


    public:
        ~SpellTimerMgr();

        void Add(uint32 name, uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast = true, bool auto_updateable = true);
        void Remove(uint32 name); // not safe to use right now
        SpellTimer* Get(uint32 name);
        void Update(uint32 const uiDiff);

        bool IsReady(uint32 name, bool isCreatureCurrentlyCasting);
        uint32 GetSpellId(uint32 name);
        void AddCooldown(uint32 name);

    private:
        SpellTimerMap m_TimerMap;

};

#endif // SPELLTIMER_H
