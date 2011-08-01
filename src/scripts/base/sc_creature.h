/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_CREATURE_H
#define SC_CREATURE_H

#include "CreatureAI.h"
#include "Creature.h"

//Spell targets used by SelectSpell
enum SelectTarget
{
    SELECT_TARGET_DONTCARE = 0,                             //All target types allowed

    SELECT_TARGET_SELF,                                     //Only Self casting

    SELECT_TARGET_SINGLE_ENEMY,                             //Only Single Enemy
    SELECT_TARGET_AOE_ENEMY,                                //Only AoE Enemy
    SELECT_TARGET_ANY_ENEMY,                                //AoE or Single Enemy

    SELECT_TARGET_SINGLE_FRIEND,                            //Only Single Friend
    SELECT_TARGET_AOE_FRIEND,                               //Only AoE Friend
    SELECT_TARGET_ANY_FRIEND,                               //AoE or Single Friend
};

//Spell Effects used by SelectSpell
enum SelectEffect
{
    SELECT_EFFECT_DONTCARE = 0,                             //All spell effects allowed
    SELECT_EFFECT_DAMAGE,                                   //Spell does damage
    SELECT_EFFECT_HEALING,                                  //Spell does healing
    SELECT_EFFECT_AURA,                                     //Spell applies an aura
};

enum SCEquip
{
    EQUIP_NO_CHANGE = -1,
    EQUIP_UNEQUIP   = 0
};

/*struct SpellCastTimer
{
    SpellCastTimer(Creature* m_creature, Unit* m_target, uint32 m_uiSpellId, uint32 m_uiSpellInterval, uint32 m_uiSpellIntervalFirst, bool m_bIsForcedCast) :
    caster(m_creature), target(m_target), SpellInterval(m_uiSpellInterval), SpellIntervalFirst(m_uiSpellIntervalFirst), SpellId(m_uiSpellId), IsForcedCast(m_bIsForcedCast), SpellTimer(0),  IsFirstCast(true) {}
    
    public:
        uint32 SpellId, SpellInterval, SpellIntervalFirst;
        bool IsFirstCast, IsForcedCast;
        Unit* target;

        void UpdateTimer(uint32 uiDiff) { SpellTimer += uiDiff; }
        uint32 GetCurrentTimer() { return SpellTimer; }
        bool CheckAndUpdate(uint32 const uiDiff);
        Creature* GetCaster() { return caster; }
        uint32 GetCurrentInterval()
        {
            return (SpellIntervalFirst && IsFirstCast) ? SpellIntervalFirst : SpellInterval;
        }		
        void ResetTimer()
        {
            SpellTimer = 0;
            if (IsFirstCast)
                IsFirstCast = false;
        }
    private:
        uint32 SpellTimer;
        Creature* caster;
};

typedef std::list<SpellCastTimer*> SpellCastTimerList;*/

#define DBC_COOLDOWN -1

struct SpellTimer
{
    SpellTimer(uint32 initialSpellId, uint32 initialTimer, int32 cooldown, bool check_cast = true) :
            initialSpellId_m(initialSpellId), initialTimer_m(initialTimer), check_cast_m(check_cast)
    {
        SetInitialCooldown(cooldown);
        ResetTimerToInitialValue();
        ResetSpellId();
    }
    SpellTimer() :
            initialSpellId_m(0), initialTimer_m(0), cooldown_m(0), check_cast_m(0) {}

    SpellTimer(uint32 initialSpellId) { SpellTimer(initialSpellId, 0, DBC_COOLDOWN, true); }
    /*SpellTimer(uint32 initialSpellId) :
        initialSpellId_m(initialSpellId), initialTimer_m(0), check_cast_m(true)
    {
        SetInitialCooldown(DBC_COOLDOWN);
        ResetTimerToInitialValue();
        ResetSpellId();
    }*/

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

struct MANGOS_DLL_DECL ScriptedAI : public CreatureAI
{
    explicit ScriptedAI(Creature* pCreature);
    ~ScriptedAI() {}

    //*************
    //CreatureAI Functions
    //*************

    //Called if IsVisible(Unit *who) is true at each *who move
    void MoveInLineOfSight(Unit*);

    //Called at each attack of m_creature by any victim
    void AttackStart(Unit*);

    // Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
    void EnterCombat(Unit*);

    //Called at stoping attack by any attacker
    void EnterEvadeMode();

    //Called at any heal cast/item used (call non implemented in mangos)
    void HealBy(Unit* pHealer, uint32 uiAmountHealed) {}

    // Called at any Damage to any victim (before damage apply)
    void DamageDeal(Unit* pDoneTo, uint32& uiDamage) {}

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) {}

    //Is unit visible for MoveInLineOfSight
    bool IsVisible(Unit *who) const;

    //Called at World update tick
    void UpdateAI(const uint32);

    //Called at creature death
    void JustDied(Unit*) {}

    //Called at creature killing another unit
    void KilledUnit(Unit*) {}

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature*) {}

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature*) {}

    // Called when hit by a spell
    void SpellHit(Unit*, const SpellEntry*) {}

    // Called when creature is spawned or respawned (for reseting variables)
    void JustRespawned();

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32, uint32) {}

    //*************
    // Variables
    //*************

    //*************
    //Pure virtual functions
    //*************

    //Called at creature reset either by death or evade
    virtual void Reset() = 0;

    //Called at creature EnterCombat
    virtual void Aggro(Unit*);

    //*************
    //AI Helper Functions
    //*************

    //Start movement toward victim
    void DoStartMovement(Unit* pVictim, float fDistance = 0, float fAngle = 0);

    //Start no movement on victim
    void DoStartNoMovement(Unit* pVictim);

    //Do melee swing of current victim if in rnage and ready and not casting
    void DoMeleeAttackIfReady();

    //Stop attack of current victim
    void DoStopAttack();

    //Cast spell by Id
    void DoCast(Unit* pVictim, uint32 uiSpellId, bool bTriggered = false);

    //Cast spell by spell info
    void DoCastSpell(Unit* pwho, SpellEntry const* pSpellInfo, bool bTriggered = false);

    //Plays a sound to all nearby players
    void DoPlaySoundToSet(WorldObject* pSource, uint32 uiSoundId);

    //Drops all threat to 0%. Does not remove players from the threat list
    void DoResetThreat();

    //Teleports a player without dropping threat (only teleports to same map)
    void DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO);

    //Returns friendly unit with the most amount of hp missing from max hp
    Unit* DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff = 1);

    //Returns a list of friendly CC'd units within range
    std::list<Creature*> DoFindFriendlyCC(float fRange);

    //Returns a list of all friendly units missing a specific buff within range
    std::list<Creature*> DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellId);

    //Return a player with at least minimumRange from m_creature
    Player* GetPlayerAtMinimumRange(float fMinimumRange);

    //Spawns a creature relative to m_creature
    Creature* DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime);

    //Returns spells that meet the specified criteria from the creatures spell list
    SpellEntry const* SelectSpell(Unit* pTarget, int32 uiSchool, int32 uiMechanic, SelectTarget selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffect);

    //Checks if you can cast the specified spell
    bool CanCast(Unit* pTarget, SpellEntry const* pSpell, bool bTriggered = false);

    void SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand = EQUIP_NO_CHANGE, int32 uiOffHand = EQUIP_NO_CHANGE, int32 uiRanged = EQUIP_NO_CHANGE);

    //Generally used to control if MoveChase() is to be used or not in AttackStart(). Some creatures does not chase victims
    void SetCombatMovement(bool bCombatMove);
    bool IsCombatMovement() { return m_bCombatMovement; }

    bool EnterEvadeIfOutOfCombatArea(const uint32 uiDiff);

    // select random players by count
    PlrList GetRandomPlayers(uint8 count, bool not_select_current_victim = false);

    // select random players prefer ranged (list of Players)
    PlrList GetRandomPlayersPreferRanged(uint8 count, uint8 min_count, float min_range, bool not_select_current_victim = false);

    // select random player prefer ranged (Player)
    Player* SelectRandomPlayerPreferRanged(uint8 min_ranged_count, float min_range, bool not_select_current_victim = false);

    // fill players list from threatlist
    PlrList GetAttackingPlayers(bool not_select_current_victim = false);

    // handle allowing creature to attack
    void EnableAttack(bool enable) { m_bAttackEnabled = enable; }

    // handles timer, if is timer prepared return true, else return false
    bool HandleTimer(uint32 &timer, const uint32 diff, bool force = false);

    // called when timed spell is casted
    void HandleTimedSpellCast(Unit* target, uint32 SpellId);

    // called after a successfull cast through periodic cast handler
    void TimedSpellCasted(Unit* target, uint32 spellId) {}

    // called to insert spell into automatic timed casting system
    void InsertTimedCast(uint32 m_uiSpellId, uint32 m_uiSpellInterval, Unit* target = NULL, uint32 m_uiSpellInvervalFirst = NULL, bool m_bForceCast = false);

    //SpellCastTimerList m_lCastTimerList;

    private:
        bool   m_bCombatMovement;
        uint32 m_uiEvadeCheckCooldown;
        bool   m_bAttackEnabled;
};

struct MANGOS_DLL_DECL Scripted_NoMovementAI : public ScriptedAI
{
    Scripted_NoMovementAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    //Called at each attack of m_creature by any victim
    void AttackStart(Unit*);
};

#endif
