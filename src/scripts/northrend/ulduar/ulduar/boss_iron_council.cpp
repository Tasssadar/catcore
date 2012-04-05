/* ScriptData
SDName: boss_iron_council
SD%Complete: 90%
SDComment: missing yells and rune of power
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells
    SAY_MOLGEIM_AGGRO       = -1603040,
    SAY_MOLGEIM_DEATH1      = -1603041,
    SAY_MOLGEIM_DEATH2      = -1603042,
    SAY_MOLGEIM_DEATH_RUNE  = -1603043,
    SAY_MOLGEIM_SUMMON      = -1603044,
    SAY_MOLGEIM_SLAY1       = -1603045,
    SAY_MOLGEIM_SLAY2       = -1603046,
    SAY_MOLGEIM_BERSERK     = -1603047,
    
    SAY_STEEL_AGGRO         = -1603050,
    SAY_STEEL_DEATH1        = -1603051,
    SAY_STEEL_DEATH2        = -1603052,
    SAY_STEEL_SLAY1         = -1603053,
    SAY_STEEL_SLAY2         = -1603054,
    SAY_STEEL_OVERWHELMING  = -1603055,
    SAY_STEEL_BERSERK       = -1603056,

    SAY_BRUNDIR_AGGR0       = -1603060,
    SAY_BRUNDIR_WHIRL       = -1603062,
    SAY_BRUNDIR_DEATH1      = -1603063,
    SAY_BRUNDIR_DEATH2      = -1603064,
    SAY_BRUNDIR_SLAY1       = -1603065,
    SAY_BRUNDIR_SLAY2       = -1603066,
    SAY_BRUNDIR_BERSERK     = -1603067,
    SAY_BRUNDIR_FLY         = -1603068,

    //all
    SPELL_BERSERK				= 47008,
    SPELL_SUPERCHARGE           = 61920,
    //steelbreaker
    SPELL_HIGH_VOLTAGE          = 61890,
    SPELL_HIGH_VOLTAGE_H        = 63498,
    SPELL_FUSION_PUNCH          = 61903,
    SPELL_FUSION_PUNCH_H        = 63493,
    SPELL_STATIC_DISRUPTION     = 44008,
    SPELL_STATIC_DISRUPTION_H   = 63494,
    SPELL_POWER                 = 64637,
    SPELL_POWER_H               = 61888,
    SPELL_ELECTRICAL_CHARGE     = 61902,
    SPELL_ELECTRICAL_CHARGE_2   = 61901,
    //runemaster molgeim
    SPELL_SHIELD                = 62274,
    SPELL_SHIELD_H              = 63489,
    SPELL_RUNE_OF_POWER         = 63513,
    SPELL_RUNE_OF_DEATH         = 62269,
    SPELL_RUNE_OF_DEATH_H       = 63490,
    SPELL_RUNE_OF_SUMMONING     = 62273,
    //rune of power
    AURA_RUNE_OF_POWER          = 61974,
    //rune of summoning
    AURA_RUNE_OF_SUMMONING      = 62019,
    //lightning elemental
    SPELL_LIGHTNING_BLAST       = 62054,
    SPELL_LIGHTNING_BLAST_H     = 63491,
    //stormcaller brundir
    SPELL_CHAIN_LIGHTNING       = 61879,
    SPELL_CHAIN_LIGHTNING_H     = 63479,
    SPELL_OVERLOAD              = 61869,
    SPELL_LIGHTNING_WHIRL       = 61915,
    SPELL_LIGHTNING_WHIRL_H     = 63483,
    SPELL_STORMSHIELD           = 64187,
    SPELL_LIGHTNING_TENDRILS    = 61887,
    SPELL_LIGHTNING_TENDRILS_H  = 63486,
    LIGHTNING_TENDRILS_VISUAL   = 61883,
    //NPC ids
    MOB_LIGHTNING_ELEMENTAL     = 32958, 

    ACHIEV_ON_YOUR_SIDE         = 2945,
    ACHIEV_ON_YOUR_SIDE_H       = 2946,
    SPELL_IRON_BOOT_AURA        = 58501,

    ACHIEV_CHOOSE_BRUNDIR       = 2940,
    ACHIEV_CHOOSE_BRUNDIR_H     = 2943,
    ACHIEV_CHOOSE_MOLGEIM       = 2939,
    ACHIEV_CHOOSE_MOLGEIM_H     = 2942,
    ACHIEV_CHOOSE_STEELBREAKER  = 2941,
    ACHIEV_CHOOSE_STEELBREAKER_H= 2944,
};

float const m_fLootAssembly[4] = {1520.0f,  112.2f, 427.3f, 6.27f};  //assembly
float const m_fLootAssemblyH[4] = {1520.8f,  126.7f, 427.3f, 6.27f}; //assembly hard

// Rune of Power
struct MANGOS_DLL_DECL mob_rune_of_powerAI : public ScriptedAI
{
    mob_rune_of_powerAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        SetCombatMovement(false);
        Reset();
    }

    uint32 m_uiDeath_Timer;

    void Reset()
    {
        m_uiDeath_Timer = 60000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  AURA_RUNE_OF_POWER);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiDeath_Timer < diff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }else m_uiDeath_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_rune_of_power(Creature* pCreature)
{
    return new mob_rune_of_powerAI(pCreature);
}

// Lightning Elemental
struct MANGOS_DLL_DECL mob_ulduar_lightning_elementalAI : public ScriptedAI
{
    mob_ulduar_lightning_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDeath_Timer;
    uint32 m_uiCheck_Timer;
    bool m_bWillExplode;

    void Reset()
    {
        m_bWillExplode  = false;
        m_uiCheck_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDeath_Timer < diff && m_bWillExplode)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDeath_Timer -= diff;

        if (m_uiCheck_Timer < diff)
        {
            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 15))
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_BLAST : SPELL_LIGHTNING_BLAST_H);
                m_bWillExplode  = true;
                m_uiDeath_Timer = 500;
                m_uiCheck_Timer = 5000;
            }
            m_uiCheck_Timer = 1000;
        }else m_uiCheck_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_ulduar_lightning_elemental(Creature* pCreature)
{
    return new mob_ulduar_lightning_elementalAI(pCreature);
}

// Rune of Summoning
struct MANGOS_DLL_DECL mob_rune_of_summoningAI : public ScriptedAI
{
    mob_rune_of_summoningAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty(); 
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDeath_Timer;
    uint32 m_uiSummon_Timer;
    uint32 m_uiSummonNum;
    uint32 m_uiCount;

    void Reset()
    {
        m_uiDeath_Timer     = 0;
        m_uiSummon_Timer    = 5000;
        m_uiSummonNum       = 0;
        m_uiCount           = m_bIsRegularMode ? 10 : 25;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  AURA_RUNE_OF_SUMMONING);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetInCombatWithZone();
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MOLGEIM)))
            if (Unit* pTarget = pTemp->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AddThreat(pTarget, 100.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiSummon_Timer < diff)
        {
            if (Creature* pTemp = m_creature->SummonCreature(MOB_LIGHTNING_ELEMENTAL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                ++m_uiSummonNum;
            
            if (m_uiSummonNum > m_uiCount)
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

            m_uiSummon_Timer = 4000/m_uiCount;
        } else m_uiSummon_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_rune_of_summoning(Creature* pCreature)
{
    return new mob_rune_of_summoningAI(pCreature);
}

//Stormcaller Brundir
struct MANGOS_DLL_DECL boss_brundirAI : public ScriptedAI
{
    boss_brundirAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty(); 
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiChain_Lightning_Timer;
    uint32 m_uiOverload_Timer;
    uint32 m_uiWhirl_Timer;
    uint32 m_uiTendrils_start_Timer;
    uint32 m_uiTendrils_Change;
    uint32 m_uiTendrils_end_Timer;
    uint32 m_uiDie_delay;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiCheckTimer;

    bool m_bHasSupercharge1;
    bool m_bHasSupercharge2;
    bool m_bIsTendrils;
    bool m_bMustDie;
    bool m_bIsSteelbreakerDead;
    bool m_bIsMolgeimDead;
    bool m_bIsEnrage;

    Creature *pSteelbreaker;
    Creature *pMolgeim;
    
    void Reset()
    {
        m_uiChain_Lightning_Timer = 0;
        m_uiOverload_Timer      = 35000;
        m_uiEnrage_Timer        = 900000;
        m_uiCheckTimer          = 1000;
        m_bIsEnrage             = false;
        m_bHasSupercharge1      = false;
        m_bHasSupercharge2      = false;
        m_bIsTendrils           = false;
        m_bIsSteelbreakerDead   = false;
        m_bIsMolgeimDead        = false;
        m_bMustDie              = false;
        pSteelbreaker           = NULL;
        pMolgeim                = NULL;
        if (m_creature->HasAura(SPELL_SUPERCHARGE))
            m_creature->RemoveAurasDueToSpell(SPELL_SUPERCHARGE);
        
        if (m_pInstance->GetData(TYPE_ASSEMBLY) == IN_PROGRESS)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && !m_bMustDie)
        {
            uiDamage = 0;
            m_creature->CastStop();
            m_creature->RemoveAllAuras();
            DoCast(m_creature, SPELL_SUPERCHARGE);
            m_uiDie_delay   = 500;
            m_bMustDie      = true;
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        for(uint8 i = 0; i < 3; ++i)
        {
            if (pSpell->Effect[i] == SPELL_EFFECT_INTERRUPT_CAST)
            {
                if (m_creature->IsNonMeleeSpellCasted(false))
                {
                    m_uiChain_Lightning_Timer += 5000;
                    if (m_uiWhirl_Timer < 5000)
                        m_uiWhirl_Timer = 5000;
                    if (m_uiOverload_Timer < 5000)
                        m_uiOverload_Timer = 5000;
                }
            }
        }
    }

    void SpellHitTarget(Unit* /*target*/, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_OVERLOAD)
            m_creature->ApplySpellImmune(SPELL_OVERLOAD, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, false);
    }

    void OnYourSide()
    {
        if (Map* pMap = m_creature->GetMap())
        {
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            if (!lPlayers.isEmpty())
            {
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                    {
                        if (pPlayer->HasAura(SPELL_IRON_BOOT_AURA, EFFECT_INDEX_0))
                            pPlayer->CompletedAchievement(m_bIsRegularMode ? ACHIEV_ON_YOUR_SIDE : ACHIEV_ON_YOUR_SIDE_H);
                    }
                }
            }
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        if (m_pInstance)
        {
            // remove supercharge from players -> spell bug
            //m_pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERCHARGE);
            // if the others are dead then give loot
            if (!pMolgeim || !pSteelbreaker)
                return;

            if (pMolgeim->isAlive() && !pSteelbreaker->isAlive())
                if (Aura* pAura = pMolgeim->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pMolgeim->isAlive() && pSteelbreaker->isAlive())
                if (Aura* pAura = pSteelbreaker->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pMolgeim->isAlive() && !pSteelbreaker->isAlive())
            {
                m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
                m_pInstance->SetData(TYPE_ASSEMBLY_HARD, DONE);
                // only the current one has loot, because loot modes are implemented in sql
                //m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                m_creature->SummonGameobject( m_bIsRegularMode ? LOOT_ASSEMBLY : LOOT_ASSEMBLY_H, 
                    m_fLootAssembly[0], m_fLootAssembly[1], m_fLootAssembly[2], m_fLootAssembly[3], 604800);
                
                // I'm on your side
                OnYourSide();
                
                // ChooseSteelbreaker
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_CHOOSE_BRUNDIR : ACHIEV_CHOOSE_BRUNDIR_H);
            }

            // else make them full hp
            if (pMolgeim->isAlive())
                pMolgeim->SetHealth(pMolgeim->GetMaxHealth());
            
            if (pSteelbreaker->isAlive())
                pSteelbreaker->SetHealth(pSteelbreaker->GetMaxHealth());
        }

        if (irand(0,1))
            DoScriptText(SAY_BRUNDIR_DEATH1, m_creature);
        else
            DoScriptText(SAY_BRUNDIR_DEATH2, m_creature);
    }

    void Aggro(Unit* /*pWho*/)
    {
        pSteelbreaker = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STEELBREAKER));
        pMolgeim = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MOLGEIM));
        if (pSteelbreaker)
        {
            if (pSteelbreaker->isAlive())
                pSteelbreaker->SetInCombatWithZone();
        }
        if (pMolgeim)
        {
            if (pMolgeim->isAlive())
                pMolgeim->SetInCombatWithZone();
        }
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
                m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);
        }

        DoScriptText(SAY_BRUNDIR_AGGR0, m_creature);
    }

    void JustReachedHome()
    {
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STEELBREAKER)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MOLGEIM)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void KilledUnit(Unit* /*who*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_BRUNDIR_SLAY1, m_creature);
        else
            DoScriptText(SAY_BRUNDIR_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // level 1 spells
        if (m_uiChain_Lightning_Timer < uiDiff && !m_bIsTendrils)
        {
            if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
            m_uiChain_Lightning_Timer = 2000;
        }else m_uiChain_Lightning_Timer -= uiDiff;   

        if (m_uiOverload_Timer < uiDiff && !m_bIsTendrils)
        {
            m_creature->CastStop();
            m_creature->ApplySpellImmune(SPELL_OVERLOAD, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            DoCast(m_creature, SPELL_OVERLOAD);
            m_uiOverload_Timer = 40000;
        }else m_uiOverload_Timer -= uiDiff;  

        // level 2 spells
        if (m_uiWhirl_Timer < uiDiff && !m_bIsTendrils && m_bHasSupercharge1)
        {
            m_creature->CastStop();
            DoScriptText(SAY_BRUNDIR_WHIRL, m_creature);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_WHIRL : SPELL_LIGHTNING_WHIRL_H);
            m_uiWhirl_Timer = 15000;
        }else m_uiWhirl_Timer -= uiDiff;

        // level 3 spells
        if (m_uiTendrils_start_Timer < uiDiff && m_bHasSupercharge2)
        {
            if (!m_bIsTendrils)
            {
                DoScriptText(SAY_BRUNDIR_FLY, m_creature);
                m_creature->CastStop();
                DoCast(m_creature, LIGHTNING_TENDRILS_VISUAL);
                if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->AddThreat(pTarget,0.0f);
                    m_creature->AI()->AttackStart(pTarget);
                }
                m_bIsTendrils = true;
                m_creature->SetSpeedRate(MOVE_RUN, 0.8f);
                m_uiTendrils_start_Timer = 3000;
                m_uiTendrils_end_Timer = 40000;
                m_uiTendrils_Change = 5000;
            } 
            else
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_TENDRILS : SPELL_LIGHTNING_TENDRILS_H);
                m_uiTendrils_start_Timer = 90000;
            }
        }else m_uiTendrils_start_Timer -= uiDiff;

        if (m_uiTendrils_Change < uiDiff && m_bIsTendrils)
        {
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->AddThreat(pTarget,0.0f);
                m_creature->AI()->AttackStart(pTarget);
            }
            m_uiTendrils_Change = 6000;
        }else m_uiTendrils_Change -= uiDiff;

        if (m_uiTendrils_end_Timer < uiDiff && m_bIsTendrils)
        {
            if (m_creature->HasAura(SPELL_LIGHTNING_TENDRILS))
                m_creature->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS);
            if (m_creature->HasAura(SPELL_LIGHTNING_TENDRILS_H))
                m_creature->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS_H);
            if (m_creature->HasAura(LIGHTNING_TENDRILS_VISUAL))
                m_creature->RemoveAurasDueToSpell(LIGHTNING_TENDRILS_VISUAL);
            m_uiTendrils_start_Timer = 90000;
            m_creature->SetSpeedRate(MOVE_RUN, 1.8f);
            m_bIsTendrils = false;
            m_uiChain_Lightning_Timer = 5000;
            m_uiOverload_Timer = 35000;
            m_uiWhirl_Timer = 10000;
        }else m_uiTendrils_end_Timer -= uiDiff;

        // die after casting supercharge
        if (m_uiDie_delay < uiDiff && m_bMustDie)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDie_delay -= uiDiff;

        if (m_uiEnrage_Timer < uiDiff && !m_bIsEnrage)
        {
            DoScriptText(SAY_BRUNDIR_BERSERK, m_creature);
            m_creature->CastStop();
            DoCast(m_creature, SPELL_BERSERK);
            m_bIsEnrage = true;
        }else m_uiEnrage_Timer -= uiDiff;

        // check if the others are dead
        if (m_uiCheckTimer < uiDiff && !m_bHasSupercharge2)
        {
            if (!m_bIsSteelbreakerDead)
            {
                if (pSteelbreaker)
                {
                    if (!pSteelbreaker->isAlive())
                    {
                        m_bIsSteelbreakerDead = true;
                        if (!m_bHasSupercharge1)
                        {
                            m_bHasSupercharge1 = true;
                            m_uiWhirl_Timer = 10000;
                        }
                        else
                        {
                            m_bHasSupercharge2 = true;
                            m_uiTendrils_start_Timer = 40000;
                            m_uiTendrils_end_Timer = 60000;
                            m_uiTendrils_Change = 6000;
                        }
                    }
                }
            }
            if (!m_bIsMolgeimDead)
            {
                if (pMolgeim)
                {
                    if (!pMolgeim->isAlive())
                    {
                        m_bIsMolgeimDead = true;
                        if (!m_bHasSupercharge1)
                        {
                            m_bHasSupercharge1 = true;
                            m_uiWhirl_Timer = 10000;
                        }
                        else
                        {
                            m_bHasSupercharge2 = true;
                            m_uiTendrils_start_Timer = 40000;
                            m_uiTendrils_end_Timer = 60000;
                            m_uiTendrils_Change = 6000;
                        }
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }else m_uiCheckTimer -= uiDiff;
        
        if (!m_bIsTendrils && !m_bMustDie)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_brundir(Creature* pCreature)
{
    return new boss_brundirAI(pCreature);
}

//Runemaster Molgeim
struct MANGOS_DLL_DECL boss_molgeimAI : public ScriptedAI
{
    boss_molgeimAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShield_Timer;
    uint32 m_uiRune_Power_Timer;
    uint32 m_uiRune_Death_Timer;
    uint32 m_uiRune_Summon_Timer;
    uint32 m_uiDie_delay;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiCheckTimer;

    bool m_bSupercharge1;
    bool m_bSupercharge2;
    bool m_bMustDie;
    bool m_bBrundirDead;
    bool m_bSteelbreakerDead;
    bool m_bEnrage;

    Creature *pBrundir;
    Creature *pSteelbreaker;
    
    void Reset()
    {
        m_uiShield_Timer    = 20000;
        m_uiRune_Power_Timer = 10000;
        m_uiEnrage_Timer    = 900000;
        m_uiCheckTimer      = 1000;
        m_bEnrage           = false;
        m_bBrundirDead      = false;
        m_bSteelbreakerDead = false;
        m_bSupercharge1     = false;
        m_bSupercharge2     = false;
        m_bMustDie          = false;
        pBrundir            = NULL;
        pSteelbreaker       = NULL;
        if (m_creature->HasAura(SPELL_SUPERCHARGE))
            m_creature->RemoveAurasDueToSpell(SPELL_SUPERCHARGE);

        if (m_pInstance->GetData(TYPE_ASSEMBLY) == IN_PROGRESS)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && !m_bMustDie)
        {
            uiDamage = 0;
            m_creature->RemoveAllAuras();
            m_creature->CastStop();
            DoCast(m_creature, SPELL_SUPERCHARGE);
            m_uiDie_delay = 500;
            m_bMustDie = true;
        }
    }

    void OnYourSide()
    {
        if (Map* pMap = m_creature->GetMap())
        {
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            if (!lPlayers.isEmpty())
            {
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                    {
                        if (pPlayer->HasAura(SPELL_IRON_BOOT_AURA, EFFECT_INDEX_0))
                            pPlayer->CompletedAchievement(m_bIsRegularMode ? ACHIEV_ON_YOUR_SIDE : ACHIEV_ON_YOUR_SIDE_H);
                    }
                }
            }
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        if (m_pInstance)
        {
            // remove supercharge from players -> spell bug
            //m_pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERCHARGE);
            // if the others are dead then give loot
            if (!pBrundir || !pSteelbreaker)
                return;

            if (pBrundir->isAlive() && !pSteelbreaker->isAlive())
                if (Aura* pAura = pBrundir->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pBrundir->isAlive() && pSteelbreaker->isAlive())
                if (Aura* pAura = pSteelbreaker->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pBrundir->isAlive() && !pSteelbreaker->isAlive())
            {
                m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
                m_pInstance->SetData(TYPE_ASSEMBLY_HARD, DONE);
                // only the current one has loot, because loot modes are implemented in sql
                //m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                m_creature->SummonGameobject( m_bIsRegularMode ? LOOT_ASSEMBLY: LOOT_ASSEMBLY_H, 
                    m_fLootAssembly[0], m_fLootAssembly[1], m_fLootAssembly[2], m_fLootAssembly[3], 604800);
                
                // I'm on your side
                OnYourSide();
                
                // ChooseSteelbreaker
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_CHOOSE_MOLGEIM : ACHIEV_CHOOSE_MOLGEIM_H);
            }

            // else make them full hp
            if (pBrundir->isAlive())
                pBrundir->SetHealth(pBrundir->GetMaxHealth());
            
            if (pSteelbreaker->isAlive())
                pSteelbreaker->SetHealth(pSteelbreaker->GetMaxHealth());
        }

        if (irand(0,1))
            DoScriptText(SAY_MOLGEIM_DEATH1, m_creature);
        else
            DoScriptText(SAY_MOLGEIM_DEATH2, m_creature);
    }

    void Aggro(Unit* /*pWho*/)
    {
        pSteelbreaker = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STEELBREAKER));
        pBrundir = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BRUNDIR));
        if (pSteelbreaker)
        {
            if (pSteelbreaker->isAlive())
                pSteelbreaker->SetInCombatWithZone();
        }
        if (pBrundir)
        {
            if (pBrundir->isAlive())
                pBrundir->SetInCombatWithZone();
        }
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
                m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);
        }

        DoScriptText(SAY_MOLGEIM_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STEELBREAKER)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BRUNDIR)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void KilledUnit(Unit* /*who*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_MOLGEIM_SLAY1, m_creature);
        else
            DoScriptText(SAY_MOLGEIM_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // level 1 spells
        if (m_uiShield_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_SHIELD : SPELL_SHIELD_H);
            m_uiShield_Timer = 50000;
        }else m_uiShield_Timer -= uiDiff;   

        if (m_uiRune_Power_Timer < uiDiff)
        {
            switch(urand(0, 2))
            {
            case 0:
                if (pBrundir)
                {
                    if (pBrundir->isAlive())
                        DoCast(pBrundir, SPELL_RUNE_OF_POWER);
                    else
                        DoCast(m_creature, SPELL_RUNE_OF_POWER);
                }
                break;
            case 1:
                if (pSteelbreaker)
                {
                    if (pSteelbreaker->isAlive())
                        DoCast(pSteelbreaker, SPELL_RUNE_OF_POWER);
                    else
                        DoCast(m_creature, SPELL_RUNE_OF_POWER);
                }
                break;
            case 2:
                DoCast(m_creature, SPELL_RUNE_OF_POWER);
                break;
            }
            m_uiRune_Power_Timer = 30000;
        }else m_uiRune_Power_Timer -= uiDiff;

        // level2 spells
        if (m_uiRune_Death_Timer < uiDiff && m_bSupercharge1)
        {
            DoScriptText(SAY_MOLGEIM_DEATH_RUNE, m_creature);
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_RUNE_OF_DEATH : SPELL_RUNE_OF_DEATH_H);
            m_uiRune_Death_Timer = 30000;
        }else m_uiRune_Death_Timer -= uiDiff;

        // level 3 spells
        if (m_uiRune_Summon_Timer < uiDiff && m_bSupercharge2)
        {
            DoScriptText(SAY_MOLGEIM_SUMMON, m_creature);
            m_creature->CastStop();
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_RUNE_OF_SUMMONING);
            m_uiRune_Summon_Timer = 30000;
        }else m_uiRune_Summon_Timer -= uiDiff;

        // die after overloading
        if (m_uiDie_delay < uiDiff && m_bMustDie)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDie_delay -= uiDiff;

        if (m_uiEnrage_Timer < uiDiff && !m_bEnrage)
        {
            DoScriptText(SAY_MOLGEIM_BERSERK, m_creature);
            m_creature->CastStop();
            DoCast(m_creature, SPELL_BERSERK);
            m_bEnrage = true;
        }else m_uiEnrage_Timer -= uiDiff;

        if (m_uiCheckTimer < uiDiff)
        {
            if (!m_bSteelbreakerDead)
            {
                if (pSteelbreaker)
                {
                    if (!pSteelbreaker->isAlive())
                    {
                        m_bSteelbreakerDead = true;
                        if (!m_bSupercharge1)
                        {
                            m_bSupercharge1 = true;
                            m_uiRune_Death_Timer = 10000;
                        }
                        else
                        {
                            m_bSupercharge2 = true;
                            m_uiRune_Summon_Timer = 20000;
                        }
                    }
                }
            }
            if (!m_bBrundirDead)
            {
                if (pBrundir)
                {
                    if (!pBrundir->isAlive())
                    {
                        m_bBrundirDead = true;
                        if (!m_bSupercharge1)
                        {
                            m_bSupercharge1 = true;
                            m_uiRune_Death_Timer = 10000;
                        }
                        else
                        {
                            m_bSupercharge2 = true;
                            m_uiRune_Summon_Timer = 20000;
                        }
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }else m_uiCheckTimer -= uiDiff;
        
        if (!m_bMustDie)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_molgeim(Creature* pCreature)
{
    return new boss_molgeimAI(pCreature);
}

//Steelbreaker
struct MANGOS_DLL_DECL boss_steelbreakerAI : public ScriptedAI
{
    boss_steelbreakerAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFusion_Punch_Timer;
    uint32 m_uiStatic_Disruption_Timer;
    uint32 m_uiPower_Timer;
    uint32 m_uiDie_delay;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiCheckTimer;
    uint32 m_uiElectricalChargeTimer;

    bool m_bBrundirDead;
    bool m_bMolgeimDead;
    bool m_bSupercharge1;
    bool m_bSupercharge2;
    bool m_bMustDie;
    bool m_bEnrage;
    bool m_bElectricalCharge;
    
    Creature *pBrundir;
    Creature *pMogleim;

    void Reset()
    {
        m_uiFusion_Punch_Timer = 20000;
        m_uiEnrage_Timer    = 900000;
        m_uiCheckTimer      = 1000;
        m_bEnrage           = false;
        m_bBrundirDead      = false;
        m_bMolgeimDead      = false;
        m_bSupercharge1     = false;
        m_bSupercharge2     = false;
        m_bMustDie          = false;
        m_bElectricalCharge = false;
        pMogleim = NULL;
        pBrundir = NULL;
        if (m_creature->HasAura(SPELL_SUPERCHARGE))
            m_creature->RemoveAurasDueToSpell(SPELL_SUPERCHARGE);
        if (m_creature->HasAura(SPELL_ELECTRICAL_CHARGE))
            m_creature->RemoveAurasDueToSpell(SPELL_ELECTRICAL_CHARGE);
        if (m_creature->HasAura(SPELL_HIGH_VOLTAGE))
            m_creature->RemoveAurasDueToSpell(SPELL_HIGH_VOLTAGE);
        if (m_creature->HasAura(SPELL_HIGH_VOLTAGE_H))
            m_creature->RemoveAurasDueToSpell(SPELL_HIGH_VOLTAGE_H);

        if (m_pInstance->GetData(TYPE_ASSEMBLY) == IN_PROGRESS)
            m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (m_bSupercharge2)
            m_creature->CastSpell(m_creature, SPELL_ELECTRICAL_CHARGE_2, true);

        if (irand(0,1))
            DoScriptText(SAY_STEEL_SLAY1, m_creature);
        else
            DoScriptText(SAY_STEEL_SLAY2, m_creature);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && !m_bMustDie)
        {
            uiDamage = 0;
            m_creature->CastStop();
            m_creature->RemoveAllAuras();
            DoCast(m_creature, SPELL_SUPERCHARGE);
            m_uiDie_delay   = 500;
            m_bMustDie      = true;
        }
    }

    void OnYourSide()
    {
        if (Map* pMap = m_creature->GetMap())
        {
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            if (!lPlayers.isEmpty())
            {
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                    {
                        if (pPlayer->HasAura(SPELL_IRON_BOOT_AURA, EFFECT_INDEX_0))
                            pPlayer->CompletedAchievement(m_bIsRegularMode ? ACHIEV_ON_YOUR_SIDE : ACHIEV_ON_YOUR_SIDE_H);
                    }
                }
            }
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        if (m_pInstance)
        {
            // remove supercharge from players -> spell bug
            //m_pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERCHARGE);
            // if the others are dead then give loot
            //Creature* pMolgeim = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MOLGEIM)));
            //Creature* pBrundir = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_BRUNDIR)));

            if (!pMogleim || !pBrundir)
                return;

            if (pMogleim->isAlive() && !pBrundir->isAlive())
                if (Aura* pAura = pMogleim->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pMogleim->isAlive() && pBrundir->isAlive())
                if (Aura* pAura = pBrundir->GetAura(SPELL_SUPERCHARGE, EFFECT_INDEX_0))
                    pAura->modStackAmount(1);

            if (!pMogleim->isAlive() && !pBrundir->isAlive())
            {
                m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
                m_pInstance->SetData(TYPE_ASSEMBLY_HARD, DONE);
                // only the current one has loot, because loot modes are implemented in sql
                //m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                m_creature->SummonGameobject( m_bIsRegularMode ? LOOT_ASSEMBLY : LOOT_ASSEMBLY_H, 
                    m_fLootAssembly[0], m_fLootAssembly[1], m_fLootAssembly[2], m_fLootAssembly[3], 604800);
                m_creature->SummonGameobject( m_bIsRegularMode ? LOOT_ASEEMBLY_HARD : LOOT_ASSEMBLY_HARD_H, 
                    m_fLootAssemblyH[0], m_fLootAssemblyH[1], m_fLootAssemblyH[2], m_fLootAssemblyH[3], 604800);

                m_creature->SetHardModeKill(true);

                // I'm on your side
                OnYourSide();
                
                // ChooseSteelbreaker
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_CHOOSE_STEELBREAKER : ACHIEV_CHOOSE_STEELBREAKER_H);
            }


            // else make them full hp
            if (pMogleim->isAlive())
                pMogleim->SetHealth(pMogleim->GetMaxHealth());
            
            if (pBrundir->isAlive())
                pBrundir->SetHealth(pBrundir->GetMaxHealth());
        }

        if (irand(0,1))
            DoScriptText(SAY_STEEL_DEATH1, m_creature);
        else
            DoScriptText(SAY_STEEL_DEATH2, m_creature);
    }

    void Aggro(Unit* /*pWho*/)
    {        
        DoCast(m_creature, m_bIsRegularMode ? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H);
        if (m_pInstance && m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
            m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);

        DoScriptText(SAY_STEEL_AGGRO, m_creature);
        pMogleim = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MOLGEIM));
        pBrundir = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BRUNDIR));
        if (pMogleim)
        {
            if (pMogleim->isAlive())
                pMogleim->SetInCombatWithZone();
        }
        if (pBrundir)
        {
            if (pBrundir->isAlive())
                pBrundir->SetInCombatWithZone();
        }
    }

    void JustReachedHome()
    {
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MOLGEIM)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (Creature* pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BRUNDIR)))
        {
            if (!pTemp->isAlive())
                pTemp->Respawn();
        }
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // level 1 spells
        if (m_uiFusion_Punch_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FUSION_PUNCH : SPELL_FUSION_PUNCH_H);
            m_uiFusion_Punch_Timer = 20000;
        }else m_uiFusion_Punch_Timer -= uiDiff;

        // level 2 spells
        if (m_uiStatic_Disruption_Timer < uiDiff && m_bSupercharge1)
        {
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_STATIC_DISRUPTION : SPELL_STATIC_DISRUPTION_H);
            m_uiStatic_Disruption_Timer = 60000;
        }else m_uiStatic_Disruption_Timer -= uiDiff;

        // level 3 spells
        if (m_uiPower_Timer < uiDiff && m_bSupercharge2)
        {
            m_creature->CastStop();
            DoScriptText(SAY_STEEL_OVERWHELMING, m_creature);
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_TOPAGGRO, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_POWER : SPELL_POWER_H);
            m_uiPower_Timer = m_bIsRegularMode ? 65000 : 35000;
            m_uiElectricalChargeTimer = m_bIsRegularMode ? 35000 : 60000;
            m_bElectricalCharge = true;

        }else m_uiPower_Timer -= uiDiff;

        if (m_uiElectricalChargeTimer < uiDiff && m_bElectricalCharge)
        {
            m_creature->CastSpell(m_creature, SPELL_ELECTRICAL_CHARGE_2, true);
            m_bElectricalCharge = false;
        }else m_uiElectricalChargeTimer -= uiDiff;

        if (m_uiDie_delay < uiDiff && m_bMustDie)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDie_delay -= uiDiff;

        if (m_uiEnrage_Timer < uiDiff && !m_bEnrage)
        {
            DoScriptText(SAY_STEEL_BERSERK, m_creature);
            m_creature->CastStop();
            DoCast(m_creature, SPELL_BERSERK);
            m_bEnrage = true;
        }else m_uiEnrage_Timer -= uiDiff;

        if (m_uiCheckTimer < uiDiff)
        {
            if (!m_bBrundirDead)
            {
                if (pBrundir)
                {
                    if (!pBrundir->isAlive())
                    {
                        m_bBrundirDead = true;
                        if (!m_bSupercharge1)
                        {
                            m_bSupercharge1 = true;
                            m_uiStatic_Disruption_Timer = 12000;
                        }
                        else
                        {
                            m_bSupercharge2 = true;
                            m_uiPower_Timer = 5000;
                        }
                    }
                }
            }
            if (!m_bMolgeimDead)
            {
                if (pMogleim)
                {
                    if (!pMogleim->isAlive())
                    {
                        m_bMolgeimDead = true;
                        if (!m_bSupercharge1)
                        {
                            m_bSupercharge1 = true;
                            m_uiStatic_Disruption_Timer = 22000;
                        }
                        else
                        {
                            m_bSupercharge2 = true;
                            m_uiPower_Timer = 5000;
                        }
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }else m_uiCheckTimer -= uiDiff;
        
        if (!m_bMustDie)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_steelbreaker(Creature* pCreature)
{
    return new boss_steelbreakerAI(pCreature);
}

void AddSC_boss_iron_council()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_brundir";
    NewScript->GetAI = GetAI_boss_brundir;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_molgeim";
    NewScript->GetAI = GetAI_boss_molgeim;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_steelbreaker";
    NewScript->GetAI = GetAI_boss_steelbreaker;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_rune_of_power";
    NewScript->GetAI = &GetAI_mob_rune_of_power;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_rune_of_summoning";
    NewScript->GetAI = &GetAI_mob_rune_of_summoning;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_ulduar_lightning_elemental";
    NewScript->GetAI = &GetAI_mob_ulduar_lightning_elemental;
    NewScript->RegisterSelf();
}
