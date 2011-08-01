/* ScriptData
SDName: boss_hodir
SD%Complete: 60%
SDComment:Auras needs core fix
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SPELL_ENRAGE                = 26662,

    SPELL_FROZEN_BLOWS          = 62478,
    SPELL_FROZEN_BLOWS_H        = 63512,
    SPELL_FREEZE                = 62469,
    SPELL_BITTER_COLD           = 62038,
    SPELL_ICICLE                = 62460,
    SPELL_ICE_SHARDS            = 65370,
    SPELL_ICICLE_DUMMY          = 62453,
    SPELL_SNOWDRIFT             = 62463,
    SPELL_FLASH_FREEZE          = 61968,
    SPELL_FLASH_FREEZE_VIS      = 62148,
    SPELL_FLASH_FREEZE_STUN     = 64175,
    SPELL_FLASH_FREEZE_KILL     = 62226,
    SPELL_FLASH_FREEZE_NPC_STUN = 61990,

    NPC_SNOWDRIFT_TARGET    = 33174,
    NPC_ICICLE              = 33169,
    NPC_SNOW_ICICLE         = 33173,
    NPC_FLASH_FREEZE        = 32926,
    NPC_FLASH_FREEZE_NPC    = 32938, 

    SAY_AGGRO               = -1603085,
    SAY_DEATH               = -1603084,
    SAY_SLAY01              = -1603083,
    SAY_SLAY02              = -1603082,
    SAY_FLASH_FREEZE        = -1603081,
    SAY_FROZEN_BLOWS        = -1603080,
    SAY_BERSERK             = -1603087,
    EMOTE_FLASH_FREEZE      = -1603360,
    EMOTE_FROZEN_BLOWS      = -1603361,

    ACHIEV_RARE_CACHE       = 3182,
    ACHIEV_RARE_CACHE_H     = 3184,
    ACHIEV_COOLEST_FRIEND   = 2963,
    ACHIEV_COOLEST_FRIEND_H = 2965,
    ACHIEV_GETTING_COLD     = 2967,
    ACHIEV_GETTING_COLD_H   = 2968,
    ACHIEV_CHEESE_FREEZE    = 2961,
    ACHIEV_CHEESE_FREEZE_H  = 2962,

    // helper npcs
    // druid
    SPELL_WRATH         = 62793,
    SPELL_STARLIGHT     = 62807, // friendly

    // shaman
    SPELL_LAVA_BURST    = 61924,
    SPELL_STORM_CLOUD   = 65123, // friendly
    SPELL_STORM_CLOUD_H = 65133,
    SPELL_STORM_POWER   = 65134, // friendly

    // mage
    SPELL_FIREBALL      = 61909,
    SPELL_TOASTY_FIRE   = 62823, // friendly -> summon
    NPC_TOASTY_FIRE     = 33342,
    SPELL_TOASTY_FIRE_A = 62821,
    SPELL_MELT_ICE      = 64528,
    SPELL_SIGNED        = 65280,

    // priest
    SPELL_SMITE         = 61923,
    SPELL_GREAT_HEAL    = 62809,    //friendly
    SPELL_DISPEL_MAGIC  = 63499,    //friendly

};

#define CENTER_X    2000.0f
#define CENTER_Y    -234.21f

#define MAX_FRIENDLY 8
bool m_bCoolestFriend;

float const m_fLootHodir[4] = {1973.0f, -199.5f, 432.7f, 3.68f};     //hodir
float const m_fLootHodirH[4] = {2032.7f, -196.4f, 432.7f, 4.71f};    //hodir hard

int32 FriendlyInfo[MAX_FRIENDLY][5] =
{
    // both
    {33325, 32941, 2017, -243, 433},
    {33328, 33332, 2010, -255, 433},
    {32893, 33331, 2002, -243, 433},
    {32897, 32948, 1995, -255, 433},

    // 25 only
    {32901, 33333, 2032, -243, 433},
    {32900, 32950, 2025, -255, 433},
    {33327, 32946, 1987, -243, 433},
    {33326, 33330, 1980, -255, 433}
};

struct MANGOS_DLL_DECL npc_hodir_friendlyAI : public ScriptedAI
{
    npc_hodir_friendlyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    bool   m_bIsFrozen;
    bool   m_bAttacked;
    uint32 m_uiUpdateCheck;
    void Reset()
    {
        m_bIsFrozen = true;
        m_bAttacked = false;
        m_uiUpdateCheck = 2000;
    }

    void JustDied(Unit* pKiller)
    {
        m_bCoolestFriend = false;
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bIsFrozen)
            return;

        if (!pWho) 
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 20.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiUpdateCheck < uiDiff)
        {
            if (!m_creature->HasAura(61990))
            {
                m_bIsFrozen = false;
                if (!m_bAttacked)
                {
                    if (Creature* pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                        if (pHodir->isInCombat())
                        {
                            AttackStart(pHodir);
                            m_bAttacked = true;
                        }
                }
            }
            else
            {
                m_bAttacked = false;
                m_bIsFrozen = true;
            }
            m_uiUpdateCheck = 2000;
        }else m_uiUpdateCheck -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_icicleAI : public ScriptedAI
{
    mob_icicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetCombatMovement(false);
        pCreature->setFaction(14);
        pCreature->SetDisplayId(11686);     // make invisible
        m_bIsWithSnow = pCreature->GetEntry() == 33173;
        m_bIsJustDamage = pCreature->GetEntry() == 33169;
        Reset();
    }

    uint32 m_uiIcicleFallTimer;
    uint32 m_uiIcicleDamageTimer;
    bool m_bIsWithSnow;
    bool m_bIsJustDamage;

    void Reset()
    {
        m_uiIcicleFallTimer = 2500;
        m_uiIcicleDamageTimer = 4500;
    }
    void AttackStart(Unit* pWho) { return; }
    void UpdateAI(const uint32 diff)
    { 
        if (m_uiIcicleFallTimer < diff)
        {
            if (uint32 spellId = m_bIsWithSnow ? 62460 : m_bIsJustDamage ? 62236 : 0)
                m_creature->CastSpell(m_creature, spellId, true);
            m_uiIcicleFallTimer = 30000;
        } else m_uiIcicleFallTimer -= diff;

        if (m_uiIcicleDamageTimer < diff)
        {
            // despawn toasty fire
            uint32 distance = m_bIsWithSnow ? 7 : 4;
            std::list<GameObject*> l_GoFire;
            GetGameObjectListWithEntryInGrid(l_GoFire, m_creature, 194300, distance);
            if (!l_GoFire.empty())
                for(std::list<GameObject*>::iterator i = l_GoFire.begin(); i != l_GoFire.end(); ++i)
                    (*i)->Delete();
            
            std::list<Creature*> l_NpcFire;
            GetCreatureListWithEntryInGrid(l_NpcFire, m_creature, 33342, distance);
            if (!l_NpcFire.empty())
                for(std::list<Creature*>::iterator i = l_NpcFire.begin(); i != l_NpcFire.end(); ++i)
                    (*i)->ForcedDespawn();

            m_uiIcicleDamageTimer = 30000;
        }else m_uiIcicleDamageTimer -= diff;
    }
};

struct MANGOS_DLL_DECL mob_toasty_fireAI : public ScriptedAI
{
    mob_toasty_fireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetDisplayId(11686);     // make invisible
        SetCombatMovement(false);
        Reset();
    }
    ScriptedInstance* m_pInstance;

    bool m_bCasted;
    void Reset()
    {
        m_bCasted = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_bCasted)
        {
            if (Creature* pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
            {
                pHodir->CastSpell(m_creature, SPELL_TOASTY_FIRE_A, true);
                m_bCasted = true;
            }
        }
    }
};

struct MANGOS_DLL_DECL mob_npc_flashFreezeAI : public ScriptedAI
{
    mob_npc_flashFreezeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetDisplayId(25865);     // invisible
        pCreature->GetMotionMaster()->MoveIdle();
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bAttacked;
    void Reset()
    {
        m_bAttacked = false;
    }

    void AttackStart(Unit* pWho) { return; }
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_bAttacked && pDoneBy->GetTypeId() == TYPEID_PLAYER)
        {
            if (Creature* pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                pHodir->AI()->AttackStart(pDoneBy);
            m_bAttacked = true;
        }
        if (uiDamage >= m_creature->GetHealth())
        {
            ThreatList tList = m_creature->getThreatManager().getThreatList();
            for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
            {
                if (Unit* unit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid()))
                    if (Aura* aur = unit->GetAura(61990, EFFECT_INDEX_0))
                        if (aur->GetCasterGUID() == m_creature->GetGUID())
                            unit->RemoveAurasDueToSpell(61990);
            }
        } 
    }
    
    void UpdateAI(const uint32 diff) { }
};

struct MANGOS_DLL_DECL boss_hodirAI : public ScriptedAI
{
    boss_hodirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiSpeedKillTimer;

    uint32 m_uiEnrageTimer;
    uint32 m_uiFlashFreezeTimer;
    uint32 m_uiFrozenBlowsTimer;
    uint32 m_uiFreezeTimer;
    uint32 m_uiIcicleTimer;
    bool m_bIsCheese;
    uint8 m_uiFriendsCount;
    uint32 m_uiTeam;

    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiStep;
    bool m_bIsCastingFlashFreeze;

    std::list<uint64> lFriendsGUIDs;
    std::list<uint64> lFreezeGUIDs;

    Player* m_pKiller;

    void Reset()
    {
        m_uiSpeedKillTimer      = 0;
        m_bCoolestFriend        = true;
        m_uiEnrageTimer         = 480000;
        m_uiFlashFreezeTimer    = 50000;
        m_uiFrozenBlowsTimer    = 60000;
        m_uiFreezeTimer         = urand(15000, 20000);
        m_uiIcicleTimer         = 10000;
        m_uiOutroTimer          = 10000;
        m_uiStep                = 1;
        m_bIsOutro              = false;
        m_bIsCheese             = true;
        m_uiFriendsCount        = m_bIsRegularMode ? 4 : 8;
        m_uiTeam                = 0;
        m_bIsCastingFlashFreeze = false;
        m_pKiller               = NULL;
        
        if (m_pInstance)
        {
            Map::PlayerList const& players = m_pInstance->instance->GetPlayers();
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (Player* plr = itr->getSource())
                    if (m_uiTeam = plr->GetTeam())
                        break;
        }

        DespawnNpc();
        SpawnNpc();
        
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_HODIR, FAIL);
    }

    void Aggro(Unit *who) 
    {
        if(m_pInstance) 
            m_pInstance->SetData(TYPE_HODIR, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);

        DoCast(m_creature, SPELL_BITTER_COLD);
    }

    void DespawnNpc()
    {
        if (!lFriendsGUIDs.empty())
            for(std::list<uint64>::iterator itr = lFriendsGUIDs.begin(); itr != lFriendsGUIDs.end(); ++itr)
                if (Creature* pTemp = (Creature*)Unit::GetUnit((*m_creature), *itr))
                    pTemp->ForcedDespawn();

        if (!lFreezeGUIDs.empty())
            for(std::list<uint64>::iterator itr = lFreezeGUIDs.begin(); itr != lFreezeGUIDs.end(); ++itr)
                if (Creature* pTemp = (Creature*)Unit::GetUnit((*m_creature), *itr))
                    pTemp->ForcedDespawn();

        lFriendsGUIDs.clear();
        lFreezeGUIDs.clear();
    }

    void SpawnNpc()
    {
        Creature* pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f);
        uint32 npcfaction = m_uiTeam == ALLIANCE ? 0 : m_uiTeam == HORDE ? 1 : urand(0,1);
        for(uint8 i = 0; i < m_uiFriendsCount; ++i)
        {
            float x = FriendlyInfo[i][2];
            float y = FriendlyInfo[i][3];
            float o = pHodir ? pHodir->GetAngle(x, y) + M_PI_F : M_PI_F/2.0f;
            if (Creature* pFriend = m_creature->SummonCreature(FriendlyInfo[i][npcfaction], x, y, FriendlyInfo[i][4], o, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                lFriendsGUIDs.push_back(pFriend->GetGUID());
                FreezeTarget(pFriend);
            }
        }
    }

    void FreezeTarget(Unit* target)
    {
        if (!target || !target->isAlive())
            return;
        
        if (target->HasAura(61990))
            target->CastSpell(target, SPELL_FLASH_FREEZE_KILL, true);

        Creature* pFreeze = m_creature->SummonCreature(NPC_FLASH_FREEZE_NPC, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (!pFreeze)
            return;
        
        // freeze target
        pFreeze->SetInCombatWith(target);
        target->SetInCombatWith(pFreeze);
        pFreeze->AddThreat(target,1.0f);
        target->AddThreat(pFreeze,1.0f);
        pFreeze->CastSpell(target, 61990, true);
        lFreezeGUIDs.push_back(pFreeze->GetGUID());
    }

    void DoOutro()
    {
        if(m_pInstance) 
        {
            m_pInstance->SetData(TYPE_HODIR, DONE);
            // spawn loot chest
            m_creature->SummonGameobject(m_bIsRegularMode ? LOOT_HODIR : LOOT_HODIR_H,
                m_fLootHodir[0], m_fLootHodir[1], m_fLootHodir[2], m_fLootHodir[3], 604800);
            if(m_uiSpeedKillTimer < 180000)
            {
                m_pInstance->SetData(TYPE_HODIR_HARD, DONE);
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_RARE_CACHE : ACHIEV_RARE_CACHE_H);
                // spawn loot chest
                m_creature->SummonGameobject(m_bIsRegularMode ? LOOT_HODIR_HARD : LOOT_HODIR_HARD_H,
                    m_fLootHodirH[0], m_fLootHodirH[1], m_fLootHodirH[2], m_fLootHodirH[3], 604800);
            }

            //if (m_bCoolestFriend)
            //    m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_COOLEST_FRIEND : ACHIEV_COOLEST_FRIEND_H);

            //if (m_bIsCheese)
            //    m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_CHEESE_FREEZE : ACHIEV_CHEESE_FREEZE_H);
        }

        m_creature->SetHardModeKill(m_uiSpeedKillTimer < 180000);
        m_creature->LogKill(m_pKiller, m_uiSpeedKillTimer);
        m_creature->ForcedDespawn();
    }

    // for debug only
    void JustDied(Unit* pKiller)
    {
        if(m_pInstance)
        {
            m_pInstance->SetData(TYPE_HODIR, DONE);
            if(m_uiSpeedKillTimer > 0)
                m_pInstance->SetData(TYPE_HODIR_HARD, DONE);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        if(m_creature->GetHealthPercent() < 1.0f || uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            m_bIsOutro = true;

            // save killer for later log handling
            if (done_by->GetTypeId() == TYPEID_PLAYER)
                m_pKiller = (Player*)done_by;
        }
    }

    void KilledUnit(Unit *who)
    {
        if(irand(0,1))
            DoScriptText(SAY_SLAY01, m_creature);
        else
            DoScriptText(SAY_SLAY02, m_creature);
    }

    void CastFinished(/*Unit* target,*/ const SpellEntry * spellInfo)
    {
        if (spellInfo->Id == SPELL_FLASH_FREEZE && m_bIsCastingFlashFreeze)
        {
            // despawn toasty fire
            std::list<GameObject*> l_GoFire;
            GetGameObjectListWithEntryInGrid(l_GoFire, m_creature, 194300, DEFAULT_VISIBILITY_INSTANCE);
            if (!l_GoFire.empty())
                for(std::list<GameObject*>::iterator i = l_GoFire.begin(); i != l_GoFire.end(); ++i)
                    (*i)->Delete();
            
            std::list<Creature*> l_NpcFire;
            GetCreatureListWithEntryInGrid(l_NpcFire, m_creature, 33342, DEFAULT_VISIBILITY_INSTANCE);
            if (!l_NpcFire.empty())
                for(std::list<Creature*>::iterator i = l_NpcFire.begin(); i != l_NpcFire.end(); ++i)
                    (*i)->ForcedDespawn();

            std::list<GameObject*> lSnowdrift;
            GetGameObjectListWithEntryInGrid(lSnowdrift, m_creature, 194173, DEFAULT_VISIBILITY_INSTANCE);

            ThreatList tList = m_creature->getThreatManager().getThreatList();
            for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
            {
                Unit* unit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
                if (!unit)
                    continue;

                if (unit->GetTypeId() == TYPEID_UNIT && !((Creature*)unit)->isPet())
                    continue;

                // is within bounding radius of Snowdrift
                bool m_bIsWithinRadius = false;
                for(std::list<GameObject*>::iterator i = lSnowdrift.begin(); i != lSnowdrift.end(); ++i)
                    if ((*i)->IsWithinBoundingRadius(unit->GetPositionX(), unit->GetPositionY()))
                        m_bIsWithinRadius = true;

                if (m_bIsWithinRadius)
                    continue;

                FreezeTarget(unit);
            }
            // Npc part
            for(std::list<uint64>::iterator itr = lFriendsGUIDs.begin(); itr != lFriendsGUIDs.end(); ++itr)
            {
                if (Unit* pFriend = Unit::GetUnit(*m_creature, *itr))
                {
                    // is within bounding radius of Snowdrift
                    bool m_bIsWithinRadius = false;
                    for(std::list<GameObject*>::iterator i = lSnowdrift.begin(); i != lSnowdrift.end(); ++i)
                        if ((*i)->IsWithinBoundingRadius(pFriend->GetPositionX(), pFriend->GetPositionY()))
                            m_bIsWithinRadius = true;

                    if (m_bIsWithinRadius)
                        continue;

                    FreezeTarget(pFriend);
                }
            }
            m_bIsCastingFlashFreeze = false;
            m_uiFrozenBlowsTimer = urand(1000, 5000);
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_bIsOutro)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            // hard mode check
            m_uiSpeedKillTimer += uiDiff;

            // Freeze
            if(m_uiFreezeTimer < uiDiff && !m_creature->IsNonMeleeSpellCasted(false))
            {
                if(Unit *target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_FREEZE);
                if (m_uiFlashFreezeTimer < 10000)
                    m_uiFreezeTimer = m_uiFlashFreezeTimer - 500;
                else
                    m_uiFreezeTimer = urand(5000, 10000);
            } else m_uiFreezeTimer -= uiDiff;

            // Flash Freeze
            if(m_uiFlashFreezeTimer < uiDiff)
            {
                DoScriptText(EMOTE_FLASH_FREEZE, m_creature);
                DoScriptText(SAY_FLASH_FREEZE, m_creature);
                DoCast(m_creature, SPELL_FLASH_FREEZE);
                m_uiFlashFreezeTimer = 50000;
                m_bIsCastingFlashFreeze = true;
                m_uiIcicleTimer = 2000;
            }
            else m_uiFlashFreezeTimer -= uiDiff;

            // Icicles
            if(m_uiIcicleTimer < uiDiff)
            {
                uint32 spellId = m_bIsCastingFlashFreeze ? 62462 : 62234;
                if (m_bIsCastingFlashFreeze)
                {
                    uint8 count = m_bIsRegularMode ? 1 : 3;
                    for(uint8 i = 0; i < count; ++i)
                        if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                            target->CastSpell(target, spellId, true);
                    
                    m_uiIcicleTimer = 9000;
                }
                else
                {
                    if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                        target->CastSpell(target, spellId, true);
                    
                    m_uiIcicleTimer = m_bIsRegularMode ? urand (3000, 5000) : urand(1500, 2000);
                }
            }
            else m_uiIcicleTimer -= uiDiff;

            // Frozen Blows
            if(m_uiFrozenBlowsTimer < uiDiff && !m_creature->IsNonMeleeSpellCasted(false))
            {
                DoScriptText(SAY_FROZEN_BLOWS, m_creature);
                DoScriptText(EMOTE_FROZEN_BLOWS, m_creature);
                DoCast(m_creature, m_bIsRegularMode ? SPELL_FROZEN_BLOWS : SPELL_FROZEN_BLOWS_H);
                m_uiFrozenBlowsTimer = urand(150000, 160000);
            }
            else m_uiFrozenBlowsTimer -= uiDiff;

            // Enrage
            if(m_uiEnrageTimer < uiDiff)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                DoCast(m_creature, SPELL_ENRAGE);
                m_uiEnrageTimer = 30000;
            }
            else m_uiEnrageTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        // outro
        if(m_bIsOutro)
        {
            switch(m_uiStep)
            {
            case 1:
                m_creature->setFaction(35);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->GetMotionMaster()->MovePoint(0, 1984.64f, -206.37f, 432.68f);
                ++m_uiStep;
                m_uiOutroTimer = 10000;
                break;
            case 3:
                DoScriptText(SAY_DEATH, m_creature);
                ++m_uiStep;
                m_uiOutroTimer = 5000;
                break;
            case 5:
                DoOutro();
                ++m_uiStep;
                m_uiOutroTimer = 10000;
                break;
            }
        }
        else return;

        if (m_uiOutroTimer <= uiDiff)
        {
            ++m_uiStep;
            m_uiOutroTimer = 330000;
        } m_uiOutroTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL npc_hodir_druidAI : public npc_hodir_friendlyAI
{
    npc_hodir_druidAI(Creature* pCreature) : npc_hodir_friendlyAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 m_uiWrathTimer;
    uint32 m_uiStarlightTimer;

    void Reset()
    {
        m_uiWrathTimer = 2000;
        m_uiStarlightTimer = urand(15000, 25000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_hodir_friendlyAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsFrozen)
            return;

        // Starlight
        if (HandleTimer(m_uiStarlightTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                pHodir->CastSpell(pHodir, SPELL_STARLIGHT, true);
                m_uiStarlightTimer = 30000;
        }

        // Wrath
        if (HandleTimer(m_uiWrathTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                DoCast(pHodir, SPELL_WRATH);
            m_uiWrathTimer = 1500;
        }
    }
};

CreatureAI* GetAI_npc_hodir_druid(Creature* pCreature)
{
    return new npc_hodir_druidAI(pCreature);
}

struct MANGOS_DLL_DECL npc_hodir_shamanAI : public npc_hodir_friendlyAI
{
    npc_hodir_shamanAI(Creature* pCreature) : npc_hodir_friendlyAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *pInstance;

    uint32 m_uiLavaTimer;
    uint32 m_uiStormCloudTimer;

    void Reset()
    {
        m_uiLavaTimer = 2000;
        m_uiStormCloudTimer = urand(20000,30000);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        npc_hodir_friendlyAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsFrozen)
            return;

        // Storm Cloud
        if (HandleTimer(m_uiStormCloudTimer, uiDiff))
        {
            if (Creature* pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                if (Player* plr = pHodir->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(plr, m_bIsRegularMode ? SPELL_STORM_CLOUD : SPELL_STORM_CLOUD_H);
            
            m_uiStormCloudTimer = urand(25000,30000);
        }

        // Lava Burst
        if (HandleTimer(m_uiLavaTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                DoCast(pHodir, SPELL_LAVA_BURST);
            m_uiLavaTimer = 2500;
        }
    }
};

CreatureAI* GetAI_npc_hodir_shaman(Creature* pCreature)
{
    return new npc_hodir_shamanAI(pCreature);
}

struct MANGOS_DLL_DECL npc_hodir_mageAI : public npc_hodir_friendlyAI
{
    npc_hodir_mageAI(Creature* pCreature) : npc_hodir_friendlyAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 m_uiFireballTimer;
    uint32 m_uiConjureTimer;
    uint32 m_uiMeltIceTimer;

    void Reset()
    {
        m_uiFireballTimer = 2000;
        m_uiConjureTimer = 9000;
        m_uiMeltIceTimer = 10000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_hodir_friendlyAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsFrozen)
            return;

        // Conjure Toasty Fire
        if (HandleTimer(m_uiConjureTimer, uiDiff))
        {
            DoCast(m_creature, SPELL_TOASTY_FIRE);
            m_uiConjureTimer = 60000;
        }

        // Melt Ice
        if (HandleTimer(m_uiMeltIceTimer, uiDiff))
        {
            Unit* target = NULL;
            Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f);
            if (pHodir)
            {
                std::list<uint64> friendly = ((boss_hodirAI*)pHodir->AI())->lFriendsGUIDs;
                for(std::list<uint64>::iterator itr = friendly.begin(); itr != friendly.end(); ++itr)
                    if (Unit* pFriend = Unit::GetUnit(*m_creature, *itr))
                        if (Aura* pAura = pFriend->GetAura(61990, EFFECT_INDEX_0))
                            if(target = pAura->GetCaster())
                                break;
            }
            else
                target = GetClosestCreatureWithEntry(m_creature, NPC_FLASH_FREEZE, 50.0f);

            if (target)
                m_creature->CastSpell(target, SPELL_MELT_ICE, false);
            
            m_uiMeltIceTimer = 2000;
        }

        // Fireball
        //HandleSimpleCast(m_uiFireballTimer, SPELL_FIREBALL, 3000, uiDiff);
        if (HandleTimer(m_uiFireballTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                DoCast(pHodir, SPELL_FIREBALL);
            m_uiFireballTimer = 3000;
        }
    }
};

CreatureAI* GetAI_npc_hodir_mage(Creature* pCreature)
{
    return new npc_hodir_mageAI(pCreature);
}

struct MANGOS_DLL_DECL npc_hodir_priestAI : public npc_hodir_friendlyAI
{
    npc_hodir_priestAI(Creature* pCreature) : npc_hodir_friendlyAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 m_uiSmiteTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiDispelTimer;

    void Reset()
    {
        m_uiSmiteTimer = 2000;
        m_uiHealTimer = urand(20000,25000);
        m_uiDispelTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_hodir_friendlyAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsFrozen)
            return;

        // Dispel Magic
        if (HandleTimer(m_uiDispelTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
            {
                ThreatList tList = pHodir->getThreatManager().getPlayerThreatList();
                for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
                {
                    Unit* pTarget = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                    if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_FREEZE))
                        DoCast(pTarget, SPELL_DISPEL_MAGIC);
                }
                m_uiDispelTimer = 2000;
            }
        }

        // Greater Heal
        if (HandleTimer(m_uiHealTimer, uiDiff))
        {
            DoCast(m_creature, SPELL_GREAT_HEAL);
            m_uiHealTimer = urand(25000,30000);
        }
    
        // Smite
        if (HandleTimer(m_uiSmiteTimer, uiDiff))
        {
            if(Creature *pHodir = GetClosestCreatureWithEntry(m_creature, NPC_HODIR, 100.0f))
                DoCast(pHodir, SPELL_SMITE);
            m_uiSmiteTimer = 2000;
        }
    }
};

CreatureAI* GetAI_npc_hodir_priest(Creature* pCreature)
{
    return new npc_hodir_priestAI(pCreature);
}

CreatureAI* GetAI_boss_hodir(Creature* pCreature)
{
    return new boss_hodirAI(pCreature);
}

CreatureAI* GetAI_mob_icicle(Creature* pCreature)
{
    return new mob_icicleAI(pCreature);
}

CreatureAI* GetAI_mob_toasty_fire(Creature* pCreature)
{
    return new mob_toasty_fireAI(pCreature);
}

CreatureAI* GetAI_mob_npc_flashFreeze(Creature* pCreature)
{
    return new mob_npc_flashFreezeAI(pCreature);
}

void AddSC_boss_hodir()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_hodir";
    newscript->GetAI = &GetAI_boss_hodir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_npc_flashFreeze";
    newscript->GetAI = &GetAI_mob_npc_flashFreeze;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_icicle";
    newscript->GetAI = &GetAI_mob_icicle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toasty_fire";
    newscript->GetAI = &GetAI_mob_toasty_fire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hodir_priest";
    newscript->GetAI = &GetAI_npc_hodir_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hodir_mage";
    newscript->GetAI = &GetAI_npc_hodir_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hodir_druid";
    newscript->GetAI = &GetAI_npc_hodir_druid;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hodir_shaman";
    newscript->GetAI = &GetAI_npc_hodir_shaman;
    newscript->RegisterSelf();
}