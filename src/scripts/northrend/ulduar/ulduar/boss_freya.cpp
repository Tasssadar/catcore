/* ScriptData
SDName: boss_freya
SD%Complete: 80%
SDComment: 
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    /* YELLS */
    // freya
    SAY_AGGRO               = -1603000,
    SAY_AGGRO_HARD          = -1603001,
    SAY_SUMMON1             = -1603002,
    SAY_SUMMON2             = -1603003,
    SAY_SUMMON3             = -1603004,
    SAY_SLAY1               = -1603005,
    SAY_SLAY2               = -1603006,
    SAY_DEATH               = -1603007,
    SAY_BERSERK             = -1603008,
    EMOTE_ALLIES_NATURE     = -1603362,
    EMOTE_LIFEBINDERS_GIFT  = -1603363,
    EMOTE_GROUND_TREMMOR    = -1603364,
    EMOTE_IRON_ROOTS        = -1603365,
    //brightleaf
    SAY_BRIGHTLEAF_AGGRO    = -1603160,
    SAY_BRIGHTLEAF_SLAY1    = -1603161,
    SAY_BRIGHTLEAF_SLAY2    = -1603162,
    SAY_BRIGHTLEAF_DEATH    = -1603163,
    //Ironbranch
    SAY_IRONBRANCH_AGGRO    = -1603170,
    SAY_IRONBRANCH_SLAY1    = -1603171,
    SAY_IRONBRANCH_SLAY2    = -1603172,
    SAY_IRONBRANCH_DEATH    = -1603173,
    //Stonebark
    SAY_STONEBARK_AGGRO     = -1603180,
    SAY_STONEBARK_SLAY1     = -1603181,
    SAY_STONEBARK_SLAY2     = -1603182,
    SAY_STONEBARK_DEATH     = -1603183,

    /* BOSS SPELLS */
    SPELL_ATTUNED_TO_NATURE         = 62519, //increases healing, start at 150 stacks
    SPELL_ATTUNED_10_STACKS         = 62525,
    SPELL_ATTUNED_2_STACKS          = 62524,
    SPELL_ATTUNED_25_STACKS         = 62521,
    SPELL_TOUCH_OF_EONAR            = 62528, //heals Freya, 6k per second
    SPELL_TOUCH_OF_EONAR_H          = 62892, //heals Freya, 24k per second
    SPELL_SUNBEAM                   = 62623,
    SPELL_SUNBEAM_H                 = 62872,
    SPELL_BERSERK                   = 47008, // 10 min

    /* HARD MODE SPELLS */
    SPELL_DRAINED_OF_POWER          = 62467,
    // brightleaf
    SPELL_UNSTABLE_ENERGY_FREYA     = 62451,
    SPELL_UNSTABLE_ENERGY_FREYA_H   = 62865,
    SPELL_BRIGHTLEAFS_ESSENCE       = 62968, //62385,
    SPELL_EFFECT_BRIGHTLEAF         = 63294,
    // ironbrach
    SPELL_STRENGHTEN_IRON_ROOTS     = 63601,
    NPC_STRENGHENED_IRON_ROOTS      = 33168,
    SPELL_IRON_ROOTS_FREYA          = 62438,
    SPELL_IRON_ROOTS_FREYA_H        = 62861,
    SPELL_IRONBRANCH_ESSENCE        = 62713, //62387,
    SPELL_EFFECT_IRONBRANCH         = 63292,
    // stonebark
    SPELL_GROUND_TREMOR_FREYA       = 62437,
    SPELL_GROUND_TREMOR_FREYA_H     = 62859,
    SPELL_STONEBARKS_ESSENCE        = 65590, //62386,
    SPELL_EFFECT_STONEBARK          = 63295,

    // elder buffs
    SPELL_BUFF_BRIGHTLEAF           = 62485,
    SPELL_BUFF_BRIGHTLEAF_H         = 65587,
    SPELL_BUFF_IRONBRANCH           = 62484,
    SPELL_BUFF_IRONBRANCH_H         = 65588,
    SPELL_BUFF_STONEBARK            = 62483,
    SPELL_BUFF_STONEBARK_H          = 65589,

    SPELL_BUFF_BRIGHTLEAF2          = 62385,
    SPELL_BUFF_BRIGHTLEAF2_H        = 65585,
    SPELL_BUFF_IRONBRANCH2          = 62387,
    SPELL_BUFF_IRONBRANCH2_H        = 65586,
    SPELL_BUFF_STONEBARK2           = 62386,
    SPELL_BUFF_STONEBARK2_H         = 65590,

    NPC_SUN_BEAM                    = 33170,
    NPC_UNSTABLE_SUN_BEAM           = 33050,

    // sanctuary adds 
    NPC_EONARS_GIFT                 = 33228,
    SPELL_LIFEBINDERS_GIFT          = 62584,    // after 12 secs, heals Freya & her allies for 30%
    SPELL_LIFEBINDERS_GIFT_H        = 64185,    // the same but for 60%
    SPELL_PHEROMONES                = 62619,    // protects from conservators grip
    NPC_HEALTHY_SPORE               = 33215,

    /* ADDS */
    // 6 waves of adds. 1 of the 3 each min 
    NPC_DETONATING_LASHER           = 32918,    // recude 2 stacks
    // spells
    SPELL_FLAME_LASH                = 62608,
    SPELL_DETONATE                  = 62598,
    SPELL_DETONATE_H                = 62937,

    NPC_ANCIENT_CONSERVATOR         = 33203,    // reduce 30 stacks
    //spells
    SPELL_CONSERVATORS_GRIP         = 62532,
    SPELL_NATURES_FURY              = 62589,
    SPELL_NATURES_FURY_H            = 63571,

    /* elemental adds */                    // each one reduces 10 stacks
    NPC_WATER_SPIRIT                = 33202,
    // spells
    SPELL_TIDAL_WAVE                = 62653,
    SPELL_TIDAL_WAVE_H              = 62935,

    NPC_STORM_LASHER                = 32919,
    // spells
    SPELL_STORMBOLT                 = 62649,
    SPELL_STORMBOLT_H               = 62938,
    SPELL_LIGHTNING_LASH            = 62648,    // 3 targets
    SPELL_LIGHTNING_LASH_H          = 62939,    // 5 targets

    NPC_SNAPLASHER                  = 32916,
    // spells
    SPELL_HARDENED_BARK             = 62664,
    SPELL_HARDENED_BARK_H           = 64191,

    // nature bomb
    NPC_NATURE_BOMB                 = 34129,
    GO_NATURE_BOMB                  = 194902,
    SPELL_NATURE_BOMB               = 64587,
    SPELL_NATURE_BOMB_H             = 64650,

    /* ELDERS */                            // used in phase 1
    ELDER_BRIGHTLEAF                = 32915,
    ELDER_IRONBRANCH                = 32913,
    ELDER_STONEBARK                 = 32914,

    // brightleaf spells
    SPELL_BRIGHTLEAF_FLUX           = 62262,
    SPELL_SOLAR_FLARE               = 62240,
    SPELL_SOLAR_FLARE_H             = 62920,
    SPELL_UNSTABLE_SUN_BEAM         = 62211,
    SPELL_UNSTABLE_SUN_BEAM_A       = 62243,
    SPELL_UNSTABLE_ENERGY           = 62217,    // cancels sun bean
    SPELL_UNSTABLE_ENERGY_H         = 62922,
    SPELL_PHOTOSYNTHESIS            = 62209,

    // ironbrach spells
    SPELL_IMPALE                    = 62310,
    SPELL_IMPALE_H                  = 62928,
    SPELL_IRON_ROOTS                = 62283,
    SPELL_IRON_ROOTS_H              = 62930,
    NPC_IRON_ROOTS                  = 33088,
    SPELL_THORM_SWARM               = 62285,
    SPELL_THORM_SWARM_H             = 62931,

    // stonebark spells
    SPELL_FIST_OF_STONE             = 62344,
    SPELL_BROKEN_BONES              = 62356,
    SPELL_GROUND_TREMOR             = 62325,
    SPELL_GROUND_TREMOR_H           = 62932,
    SPELL_PETRIFIED_BARK            = 62337,
    SPELL_PETRIFIED_BARK_H          = 62933,

    // 10 man
    SPELL_SUMMON_CHEST_0            = 62950,
    SPELL_SUMMON_CHEST_1            = 62952,
    SPELL_SUMMON_CHEST_2            = 62953,
    SPELL_SUMMON_CHEST_3            = 62954,
    // 25 man
    SPELL_SUMMON_CHEST_0_H          = 62955,
    SPELL_SUMMON_CHEST_1_H          = 62956,
    SPELL_SUMMON_CHEST_2_H          = 62957,
    SPELL_SUMMON_CHEST_3_H          = 62958,

    SPELL_SUMMON_ALLIES_OF_NATURE   = 62678, //better do that in sd2
    SPELL_SUMMON_LASHERS            = 62688, // lashers - broken
    SPELL_SUMMON_ELEMENTALS         = 62686, // elementals -> better in sd2
    SPELL_SUMMON_CONSERVATOR        = 62685, // conservator
    SPELL_LIFEBINDERS_GIFT_SUMMON   = 62869,
    SPELL_NATURE_BOMB_SUMMON        = 64606,

    SPELL_SPORE_SUMMON_NE           = 62591,
    SPELL_SPORE_SUMMON_SE           = 62592,
    SPELL_SPORE_SUMMON_SW           = 62593,
    SPELL_SPORE_SUMMON_NW           = 62582,

    SPELL_HEALTHY_SPORE_VISUAL      = 62538,
    SPELL_NATURE_BOMB_VISUAL        = 64604,
    SPELL_LIFEBINDERS_VISUAL        = 62579,
    SPELL_LIFEBINDER_GROW           = 44833,

    SPELL_PHEROMONES_LG             = 62619,
    SPELL_POTENT_PHEROMONES         = 62541,
    SPELL_POTENT_PHEROMONES_PROC    = 64321,

    ACHIEV_BACK_TO_NATURE           = 2982,
    ACHIEV_BACK_TO_NATURE_H         = 2983,
    ACHIEV_KNOCK_WOOD               = 3177,
    ACHIEV_KNOCK_WOOD_H             = 3185,
    ACHIEV_KNOCK_KNOCK_WOOD         = 3178,
    ACHIEV_KNOCK_KNOCK_WOOD_H       = 3186,
    ACHIEV_KNOCK_KNOCK_KNOCK_WOOD   = 3179,
    ACHIEV_KNOCK_KNOCK_KNOCK_WOOD_H = 3187,

    TIMER_BUFFS                     = 1
};

float const m_fLootFreya[4] = {2366.3f,  -52.7f, 424.7f, 3.14f};     //freya

// Iron roots & stranghned iron roots
struct MANGOS_DLL_DECL mob_iron_rootsAI : public ScriptedAI
{
    mob_iron_rootsAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint64 m_uiVictimGUID;
    uint32 m_uiCreatureEntry;

    void Reset()
    {
        m_uiVictimGUID = 0;
        m_uiCreatureEntry = m_creature->GetEntry();
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
        {
            if (m_uiVictimGUID)
            {
                if (Unit* pVictim = Unit::GetUnit((*m_creature), m_uiVictimGUID))
                {
                    switch(m_uiCreatureEntry)
                    {
                        case NPC_IRON_ROOTS:
                            pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H);
                            break;
                        case NPC_STRENGHENED_IRON_ROOTS:
                            pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H);
                            break;
                    }
                }
            }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim) 
        {
            switch(m_uiCreatureEntry)
            {
                case NPC_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H);
                    break;
                case NPC_STRENGHENED_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H);
                    break;
            }
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (Unit* pVictim = Unit::GetUnit((*m_creature), m_uiVictimGUID))
        {
            switch(m_uiCreatureEntry)
            {
                case NPC_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H);
                    break;
                case NPC_STRENGHENED_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

// Elder Brightleaf
struct MANGOS_DLL_DECL boss_elder_brightleafAI : public ScriptedAI
{
    boss_elder_brightleafAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiBrightleafFluxTimer;
    uint32 m_uiSolarFlareTimer;
    uint32 m_uiUnstableSunBeanTimer;
    uint32 m_uiUnstabelEnergyTimer;
    uint32 m_uiSunbeamStacks;
    uint32 m_uiHealTimer;
    bool m_bHasSunbeam;

    void Reset()
    {
        m_uiBrightleafFluxTimer     = 5000;
        m_uiSolarFlareTimer         = 10000 + urand(1000, 5000);
        m_uiUnstableSunBeanTimer    = 15000;
        m_uiUnstabelEnergyTimer     = 30000;
        m_uiSunbeamStacks           = 1;
        m_bHasSunbeam               = false;
        m_creature->SetRespawnDelay(7*DAY);
    }

    void Aggro(Unit* /*pWho*/)
    {    
        DoScriptText(SAY_BRIGHTLEAF_AGGRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_BRIGHTLEAF_SLAY1, m_creature);
        else
            DoScriptText(SAY_BRIGHTLEAF_SLAY2, m_creature);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_BRIGHTLEAF_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // this needs core suport
        if (m_uiBrightleafFluxTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_BRIGHTLEAF_FLUX);
            m_uiBrightleafFluxTimer = 5000;
        }
        else m_uiBrightleafFluxTimer -= uiDiff;

        if (m_uiSolarFlareTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SOLAR_FLARE : SPELL_SOLAR_FLARE_H);
            m_uiSolarFlareTimer = 10000 + urand(1000, 5000);
        }
        else m_uiSolarFlareTimer -= uiDiff;

        // also the following spells need some core support -> hacky way of use
        // PLEASE FIX FOR REVISION!
        if (m_uiUnstableSunBeanTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_UNSTABLE_SUN_BEAM);
            m_bHasSunbeam = true;
            m_uiHealTimer = 1000;
            m_uiUnstableSunBeanTimer = urand(7000, 12000);
        }
        else m_uiUnstableSunBeanTimer -= uiDiff;

        // cast after the unstable sun bean
        if (m_uiHealTimer < uiDiff && m_bHasSunbeam)
        {
            DoCast(m_creature, SPELL_PHOTOSYNTHESIS);
            m_bHasSunbeam = false;
        }
        else m_uiHealTimer -= uiDiff;

        // removes photosynthesis when standing inside
        if (m_uiUnstabelEnergyTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY: SPELL_UNSTABLE_ENERGY_H);
            m_creature->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM_A);
            m_creature->RemoveAurasDueToSpell(SPELL_PHOTOSYNTHESIS);
            m_uiSunbeamStacks = 1;
            m_uiUnstabelEnergyTimer = urand(20000, 30000);
        }
        else m_uiUnstabelEnergyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_elder_brightleaf(Creature* pCreature)
{
    return new boss_elder_brightleafAI(pCreature);
}

// Elder Ironbranch
struct MANGOS_DLL_DECL boss_elder_ironbranchAI : public ScriptedAI
{
    boss_elder_ironbranchAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiImpaleTimer;
    uint32 m_uiIronrootsTimer;
    uint32 m_uiThornSwarmTimer;

    void Reset()
    {
        m_uiImpaleTimer         = urand(40,50)*IN_MILLISECONDS;;
        m_uiIronrootsTimer      = urand(8,10)*IN_MILLISECONDS;
        m_uiThornSwarmTimer     = urand(4,6)*IN_MILLISECONDS;
        m_creature->SetRespawnDelay(7*DAY);
    }

    void Aggro(Unit* /*pWho*/)
    {    
        DoScriptText(SAY_IRONBRANCH_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_IRONBRANCH_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_IRONBRANCH_SLAY1, m_creature);
        else
            DoScriptText(SAY_IRONBRANCH_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiImpaleTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H);
            m_uiImpaleTimer = urand(35,50)*IN_MILLISECONDS;
        }
        else m_uiImpaleTimer -= uiDiff;

        if (m_uiIronrootsTimer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                target->CastSpell(m_creature, m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H,true);
            m_uiIronrootsTimer = urand(15, 25)*IN_MILLISECONDS;;
        }
        else m_uiIronrootsTimer -= uiDiff;

        if (m_uiThornSwarmTimer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                DoCast(target, m_bIsRegularMode ? SPELL_THORM_SWARM : SPELL_THORM_SWARM_H);
            m_uiThornSwarmTimer = urand(7,9)*IN_MILLISECONDS;
        }
        else m_uiThornSwarmTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_boss_elder_ironbranch(Creature* pCreature)
{
    return new boss_elder_ironbranchAI(pCreature);
}

// Stonebark
struct MANGOS_DLL_DECL boss_elder_stonebarkAI : public ScriptedAI
{
    boss_elder_stonebarkAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiFistsOfStoneTimer;
    uint32 m_uiGroundTremorTimer;
    uint32 m_uiPetrifiedBarkTimer;

    void Reset()
    {
        m_uiFistsOfStoneTimer   = urand(13,16)*IN_MILLISECONDS;
        m_uiGroundTremorTimer   = urand(7,10)*IN_MILLISECONDS;
        m_uiPetrifiedBarkTimer  = urand(30,40)*IN_MILLISECONDS;
        m_creature->SetRespawnDelay(7*DAY);
    }

    void Aggro(Unit* /*pWho*/)
    {    
        DoScriptText(SAY_STONEBARK_AGGRO, m_creature);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_STONEBARK_DEATH, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_STONEBARK_SLAY1, m_creature);
        else
            DoScriptText(SAY_STONEBARK_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFistsOfStoneTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_FIST_OF_STONE);
            m_uiFistsOfStoneTimer = urand(35,50)*IN_MILLISECONDS;
        }
        else m_uiFistsOfStoneTimer -= uiDiff;

        if (m_uiGroundTremorTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_GROUND_TREMOR : SPELL_GROUND_TREMOR_H);
            m_uiGroundTremorTimer = urand(15,20)*IN_MILLISECONDS;
        }
        else m_uiGroundTremorTimer -= uiDiff;

        if (m_uiPetrifiedBarkTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_PETRIFIED_BARK : SPELL_PETRIFIED_BARK_H);
            m_uiPetrifiedBarkTimer = urand(25,35)*IN_MILLISECONDS;
        }
        else m_uiPetrifiedBarkTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_boss_elder_stonebark(Creature* pCreature)
{
    return new boss_elder_stonebarkAI(pCreature);
}

// Freya
struct MANGOS_DLL_DECL boss_freyaAI : public ScriptedAI
{
    boss_freyaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiWaveNumber;
    uint32 m_uiWaveType;
    uint32 m_uiWaveTypeInc;
    uint32 m_uiSunbeamTimer;
    uint32 m_uiEnrageTimer;

    bool m_bIsHardMode;
    
    uint32 m_uiNatureBombTimer;
    uint32 m_uiLifebindersGiftTimer;

    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiStep;

    // hard mode timers
    uint32 m_uiUnstableEnergyTimer;
    uint32 m_uiStrenghtenIronRootsTimer;
    uint32 m_uiGroundTremorTimer;

    uint32 m_uiThreeWaveCheckTimer;
    uint32 m_uiWaveDeadCheckTimer;
    bool m_bWaveCheck;
    bool m_bWaveDeadCheck;
    uint64 m_uiWaterSpiritGUID;
    uint64 m_uiStormLasherGUID;
    uint64 m_uiSnapLasherGUID;

    bool m_bIsBrightleafAlive;
    bool m_bIsIronbranchAlive;
    bool m_bIsStonebarkAlive;

    bool m_bNature;

    uint8 m_uiBombSummonedCount;

    bool m_bPhase2;
    Player* m_pKiller;

    void Reset()
    {
        m_uiSummonTimer                 = 15000;
        m_uiWaveNumber                  = 0;
        m_uiWaveType                    = irand(0,2);
        m_uiWaveTypeInc                 = irand(1,2);
        m_uiSunbeamTimer                = rand()%10000;
        m_uiEnrageTimer                 = 600000; //10 minutes
        m_bIsHardMode                   = false;
        m_uiLifebindersGiftTimer        = 30000;
        m_uiUnstableEnergyTimer         = 25000;
        m_uiStrenghtenIronRootsTimer    = 25000 + urand(1000, 5000);
        m_uiGroundTremorTimer           = 20000;
        m_uiNatureBombTimer             = 7000;
        m_uiThreeWaveCheckTimer         = 1000;
        m_uiWaveDeadCheckTimer          = 0;
        m_bWaveCheck                    = false;
        m_bWaveDeadCheck                = false;
        m_uiWaterSpiritGUID             = 0;
        m_uiStormLasherGUID             = 0;
        m_uiSnapLasherGUID              = 0;

        m_uiOutroTimer                  = 10000;
        m_uiStep                        = 1;
        m_bIsOutro                      = false;

        m_bNature                       = false;

        m_bPhase2                       = false;

        m_uiBombSummonedCount           = 0;

        m_pKiller                       = NULL;

        m_bIsBrightleafAlive            = false;
        m_bIsIronbranchAlive            = false;
        m_bIsStonebarkAlive             = false;

        // remove elder auras
        if (m_pInstance)
        {
            if (Creature* pBrightleaf = m_pInstance->GetCreature(NPC_BRIGHTLEAF))
                if (pBrightleaf->isAlive())
                    pBrightleaf->RemoveAllAuras();

            if (Creature* pIronbranch = m_pInstance->GetCreature(NPC_IRONBRACH))
                if (pIronbranch->isAlive())
                    pIronbranch->RemoveAllAuras();

            if (Creature* pStonebark = m_pInstance->GetCreature(NPC_STONEBARK))
                if (pStonebark->isAlive())
                    pStonebark->RemoveAllAuras();
        }

        DespawnAllWithEntry(NPC_EONARS_GIFT, TYPEID_UNIT);

        AddNonCastTimer(TIMER_BUFFS, 1000, 99999);

        if (m_pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
            m_pInstance->SetData(TYPE_FREYA, FAIL);
    }

    void Aggro(Unit* /*who*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FREYA, IN_PROGRESS);

            // check brightleaf
            if (Creature* pBrightleaf = GetNpc(NPC_BRIGHTLEAF))
                m_bIsBrightleafAlive = pBrightleaf->isAlive();

            // check ironbranch
            if (Creature* pIronbranch = GetNpc(NPC_IRONBRACH))
                m_bIsIronbranchAlive = pIronbranch->isAlive();

            // check stonebark
            if (Creature* pStonebark = GetNpc(NPC_STONEBARK))
                m_bIsStonebarkAlive = pStonebark->isAlive();
        }

        m_bIsHardMode = CheckHardMode();

        if (!m_bIsHardMode)
            DoScriptText(SAY_AGGRO, m_creature);
        else
            DoScriptText(SAY_AGGRO_HARD, m_creature);
    }

    Creature* GetNpc(uint32 Entry)
    {
        Creature* c = m_pInstance->GetCreature(Entry);
        if (!c)
            c = GetClosestCreatureWithEntry(m_creature, Entry, DEFAULT_VISIBILITY_INSTANCE);

        return c;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FREYA, FAIL);
    }

    void DoOutro()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FREYA_HARD, 0);

            uint8 m_uiAchievProgress = 0;
            if (m_bIsBrightleafAlive)
                ++m_uiAchievProgress;
            if (m_bIsIronbranchAlive)
                ++m_uiAchievProgress;
            if (m_bIsStonebarkAlive)
                ++m_uiAchievProgress;

            m_pInstance->SetData(TYPE_FREYA_HARD, m_uiAchievProgress);

            // hacky way to complete achievements; use only if you have this function
            if (m_uiAchievProgress == 1)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_KNOCK_WOOD : ACHIEV_KNOCK_WOOD_H);
            else if (m_uiAchievProgress == 2)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_KNOCK_KNOCK_WOOD : ACHIEV_KNOCK_KNOCK_WOOD_H);
            else if (m_uiAchievProgress == 3)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_KNOCK_KNOCK_KNOCK_WOOD : ACHIEV_KNOCK_KNOCK_KNOCK_WOOD_H);

            if (m_bNature)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_BACK_TO_NATURE : ACHIEV_BACK_TO_NATURE_H);
      
            uint32 m_uiLootChestId = m_bIsRegularMode ? LOOT_FREYA : LOOT_FREYA_H;
            if (m_bIsHardMode)
                m_uiLootChestId =  m_bIsRegularMode ? LOOT_FREYA_HARD : LOOT_FREYA_HARD_H;

            // spawn loot chest
            m_creature->SummonGameobject(m_uiLootChestId, m_fLootFreya[0], m_fLootFreya[1], m_fLootFreya[2], m_fLootFreya[3], 604800);

            m_pInstance->SetData(TYPE_FREYA, DONE);

            m_creature->SetHardModeKill(m_bIsHardMode);
            m_creature->LogKill(m_pKiller, m_uiAchievProgress);
            m_creature->ForcedDespawn();
        }
    }

    // for debug only!
    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FREYA, DONE);
            if (m_bIsHardMode)
                m_pInstance->SetData(TYPE_FREYA_HARD, DONE);
        }
    }

    void DamageTaken(Unit* done_by, uint32 &uiDamage)
    {
        if (m_creature->GetHealthPercent() < 1.0f || uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            m_bIsOutro = true;
            
            // save killer for later log handling
            if (done_by->GetTypeId() == TYPEID_PLAYER)
                m_pKiller = (Player*)done_by;
        }
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (irand(0,1))
            DoScriptText(SAY_SLAY1, m_creature);
        else
            DoScriptText(SAY_SLAY2, m_creature);
    }

    // summon 12 Lashers. Should be done by a spell which needs core fix
    void SummonLashers()
    {
        DoScriptText(SAY_SUMMON3, m_creature);
        for(uint8 i = 0; i < 12; ++i)
        {
            Coords coord = m_creature->GetPosition();
            float angle = rand_norm_f()*2*M_PI_F;
            float radius = urand(5,15);
            coord.x += cos(angle)*radius;
            coord.y += sin(angle)*radius;
            if (Creature* pLasher = m_creature->SummonCreature(NPC_DETONATING_LASHER, coord, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000, true))
                if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    pLasher->AddThreat(pTarget, 1.0f);
        }
    }

    // summon conservator. Should be done by a spell which needs core fix
    void SummonConservator()
    {
        DoScriptText(SAY_SUMMON1, m_creature);
        Coords coord = m_creature->GetPosition();
        float angle = rand_norm_f()*2*M_PI_F;
        float radius = urand(5,15);
        coord.x += cos(angle)*radius;
        coord.y += sin(angle)*radius;
        if (Creature* pAdd = m_creature->SummonCreature(NPC_ANCIENT_CONSERVATOR, coord, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000, true))
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                pAdd->AddThreat(pTarget, 1.0f);
    }

    // summmon the 3 elementals. Should be done by a spell which needs core fix.
    void SummonElementals()
    {
        DoScriptText(SAY_SUMMON2, m_creature);
        m_bWaveCheck = true;
        m_uiThreeWaveCheckTimer = 2000;

        if (Creature* pWaterSpirit = DoSpawnCreature(NPC_WATER_SPIRIT, 10, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0, true))
        {
            m_uiWaterSpiritGUID = pWaterSpirit->GetGUID();
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                pWaterSpirit->AddThreat(pTarget, 1.0f);
        }

        if (Creature* pStormLasher = DoSpawnCreature(NPC_STORM_LASHER, 10, 10, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0, true))
        {
            m_uiStormLasherGUID = pStormLasher->GetGUID();
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                pStormLasher->AddThreat(pTarget, 1.0f);
        }

        if (Creature* pSnapLasher = DoSpawnCreature(NPC_SNAPLASHER, -10, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0, true))
        {
            m_uiSnapLasherGUID = pSnapLasher->GetGUID();
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                pSnapLasher->AddThreat(pTarget, 1.0f);
        }
    }

    bool CheckHardMode() { return m_bIsBrightleafAlive && m_bIsIronbranchAlive && m_bIsStonebarkAlive; }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bIsOutro)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            // buffs
            if (m_TimerMgr->TimerFinished(TIMER_BUFFS))
            {
                m_TimerMgr->SetValue(TIMER_BUFFS, TIMER_VALUE_UPDATEABLE, false);

                m_creature->CastSpell(m_creature, SPELL_ATTUNED_TO_NATURE, true);
                m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_TOUCH_OF_EONAR : SPELL_TOUCH_OF_EONAR_H, true);

                // hacky way of stacking aura. Please remove when fixed in core!
                if (Aura* natureAura = m_creature->GetAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0))
                    natureAura->SetStackAmount(150);

                // brightleaf buff
                if (m_bIsBrightleafAlive)
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_BUFF_BRIGHTLEAF2 : SPELL_BUFF_BRIGHTLEAF2_H, true);

                // ironbranch buff
                if (m_bIsIronbranchAlive)
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_BUFF_IRONBRANCH2 : SPELL_BUFF_IRONBRANCH2_H, true);

                // stonebark buff
                if (m_bIsStonebarkAlive)
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_BUFF_STONEBARK2 : SPELL_BUFF_STONEBARK2_H, true);

                if (Creature* pBrightleaf = GetNpc(NPC_BRIGHTLEAF))
                    pBrightleaf->CastSpell(pBrightleaf, SPELL_DRAINED_OF_POWER, true);

                if (Creature* pIronbranch = GetNpc(NPC_IRONBRACH))
                    pIronbranch->CastSpell(pIronbranch, SPELL_DRAINED_OF_POWER, true);

                if (Creature* pStonebark = GetNpc(NPC_STONEBARK))
                    pStonebark->CastSpell(pStonebark, SPELL_DRAINED_OF_POWER, true);
            }

            // check if the 3 elementals die at the same time
            if (m_uiThreeWaveCheckTimer < uiDiff && m_bWaveCheck)
            {
                Creature* pWaterSpirit = m_pInstance->instance->GetCreature(m_uiWaterSpiritGUID);
                Creature* pStormLasher = m_pInstance->instance->GetCreature(m_uiStormLasherGUID);
                Creature* pSnapLasher = m_pInstance->instance->GetCreature(m_uiSnapLasherGUID);

                if (pWaterSpirit && pStormLasher && pSnapLasher)
                {
                    if (!pWaterSpirit->isAlive() || !pStormLasher->isAlive() || !pSnapLasher->isAlive())
                    {
                        m_bWaveCheck = false;
                        m_bWaveDeadCheck = true;
                        m_uiWaveDeadCheckTimer = 10000;
                    }
                }
                m_uiThreeWaveCheckTimer = 2000;
            }
            else
                m_uiThreeWaveCheckTimer -= uiDiff;

            // check if all 3 are dead after 10 secs
            if (m_uiWaveDeadCheckTimer < uiDiff && m_bWaveDeadCheck)
            {
                Creature* pWaterSpirit = m_pInstance->instance->GetCreature(m_uiWaterSpiritGUID);
                Creature* pStormLasher = m_pInstance->instance->GetCreature(m_uiStormLasherGUID);
                Creature* pSnapLasher = m_pInstance->instance->GetCreature(m_uiSnapLasherGUID);

                if (pWaterSpirit && pStormLasher && pSnapLasher)
                {
                    if (!pWaterSpirit->isAlive() && !pStormLasher->isAlive() && !pSnapLasher->isAlive())
                    {
                        m_bWaveDeadCheck = false;
                        pWaterSpirit->ForcedDespawn();
                        pStormLasher->ForcedDespawn();
                        pSnapLasher->ForcedDespawn();
                        if (Aura* natureAura = m_creature->GetAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0))
                        {
                            if (natureAura->modStackAmount(-30))
                            {
                                m_bPhase2 = true;
                                m_creature->RemoveAurasDueToSpell(SPELL_ATTUNED_TO_NATURE);
                            }
                        }
                    }
                    else
                    {
                        m_bWaveCheck			= true;
                        m_bWaveDeadCheck		= false;
                        m_uiThreeWaveCheckTimer = 2000;
                        // respawn the dead ones
                        if (!pWaterSpirit->isAlive())
                            pWaterSpirit->Respawn();
                        if (!pSnapLasher->isAlive())
                            pSnapLasher->Respawn();
                        if (!pStormLasher->isAlive())
                            pStormLasher->Respawn();
                    }
                }
            }
            else m_uiWaveDeadCheckTimer -= uiDiff;

            // Hardmode
            if (m_bIsBrightleafAlive)
            {
                //if (!m_creature->HasAura(SPELL_BRIGHTLEAFS_ESSENCE, EFFECT_INDEX_0))
                //    DoCast(m_creature, SPELL_BRIGHTLEAFS_ESSENCE);

                // hacky way, should be done by spell
                if (m_uiUnstableEnergyTimer < uiDiff)
                {
                    //DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY_FREYA : SPELL_UNSTABLE_ENERGY_FREYA_H);
                    for(int8 i = 0; i < 3; ++i)
                    {
                        if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                            m_creature->SummonCreature(NPC_SUN_BEAM, target->GetPosition(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 20000, true);
                    }
                    m_uiUnstableEnergyTimer = urand(25000, 30000);
                }
                else m_uiUnstableEnergyTimer -= uiDiff;
            }

            if (m_bIsIronbranchAlive)
            {
                //if (!m_creature->HasAura(SPELL_IRONBRANCH_ESSENCE, EFFECT_INDEX_0))
                //    DoCast(m_creature, SPELL_IRONBRANCH_ESSENCE);

                if (m_uiStrenghtenIronRootsTimer < uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                    {
                        DoScriptText(EMOTE_IRON_ROOTS, m_creature, target);
                        DoCast(target, m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H);
                    }
                    m_uiStrenghtenIronRootsTimer = 50000 + urand(10000, 20000);
                }
                else m_uiStrenghtenIronRootsTimer -= uiDiff;
            }

            if (m_bIsStonebarkAlive)
            {
                // aura doesn't work. Needs core fix
                //if (!m_creature->HasAura(SPELL_STONEBARKS_ESSENCE, EFFECT_INDEX_0))
                //    DoCast(m_creature, SPELL_STONEBARKS_ESSENCE);

                if (m_uiGroundTremorTimer < uiDiff)
                {
                    DoScriptText(EMOTE_GROUND_TREMMOR, m_creature);
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_GROUND_TREMOR_FREYA : SPELL_GROUND_TREMOR_FREYA_H);
                    m_uiGroundTremorTimer = 20000;
                }
                else m_uiGroundTremorTimer -= uiDiff;
            }

            //Phase 1, waves of adds
            if (m_uiWaveNumber < 6)
            {
                if (m_uiSummonTimer < uiDiff)
                {
                    DoScriptText(EMOTE_ALLIES_NATURE, m_creature);
                    switch(m_uiWaveType)
                    {
                    case 0: SummonLashers(); break;
                    case 1: SummonConservator(); break;
                    case 2: SummonElementals(); break;
                    }
                    m_uiWaveType = (m_uiWaveType + m_uiWaveTypeInc) % 3;
                    ++m_uiWaveNumber;
                    m_uiSummonTimer = 60000;
                }
                else m_uiSummonTimer -= uiDiff;
            }

            // Phase 2
            if (m_bPhase2)
            {
                // nature bomb. Should be done by spell, not by summon.
                if (m_uiNatureBombTimer < uiDiff)
                {
                    // summons only 3 bombs, just as visual
                    m_creature->CastSpell(m_creature, SPELL_NATURE_BOMB_VISUAL, true);
                    
                    GameObjectList m_lBombs;
                    GetGameObjectListWithEntryInGrid(m_lBombs, m_creature, GO_NATURE_BOMB, 200.0f);
                    if (!m_lBombs.empty())
                        for(GameObjectList::iterator itr = m_lBombs.begin(); itr != m_lBombs.end(); ++itr)
                            (*itr)->Delete();

                    uint8 bombsCount = (m_bIsRegularMode ? 3 : 5)*m_uiBombSummonedCount++;
                    PlrList targetList = GetRandomPlayersInRange(bombsCount, 0, 0, 50);
                    for(PlrList::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                        m_creature->SummonCreature(NPC_NATURE_BOMB, (*itr)->GetPosition(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 20000, true);

                    m_uiNatureBombTimer = urand(9500, 10500);
                }
                else m_uiNatureBombTimer -= uiDiff;
            }

            //All phases
            if (m_uiSunbeamTimer < uiDiff)
            {
                if ( Unit *target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_SUNBEAM : SPELL_SUNBEAM_H);
                m_uiSunbeamTimer = 6000 + rand()%10000;
            }
            else m_uiSunbeamTimer -= uiDiff;

            if (m_uiLifebindersGiftTimer < uiDiff)
            {
                DoScriptText(EMOTE_LIFEBINDERS_GIFT, m_creature);
                if (Unit *pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_LIFEBINDERS_GIFT_SUMMON);
                m_uiLifebindersGiftTimer = 30000;
            }
            else m_uiLifebindersGiftTimer -= uiDiff;

            if (m_uiEnrageTimer < uiDiff)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                DoCast(m_creature, SPELL_BERSERK);
                m_uiEnrageTimer = 30000;
            }
            else m_uiEnrageTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        // outro
        if (m_bIsOutro)
        {
            switch(m_uiStep)
            {
            case 1:
                if (m_creature->HasAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0))
                {
                    if (m_creature->GetAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0)->GetStackAmount() >= 25)
                        m_bNature = true;
                }
                m_creature->setFaction(35);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->GetMotionMaster()->MovePoint(0, 2359.40f, -52.39f, 425.64f);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                ++m_uiStep;
                m_uiOutroTimer = 7000;
                break;
            case 3:
                DoScriptText(SAY_DEATH, m_creature);
                ++m_uiStep;
                m_uiOutroTimer = 10000;
                break;
            case 5:
                DoOutro();
                ++m_uiStep;
                m_uiOutroTimer = 1000;
                break;
            case 7:
                ++m_uiStep;
                m_creature->ForcedDespawn();
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

CreatureAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

// Script for all the npcs found on the ground during Freya encounter
struct MANGOS_DLL_DECL mob_freya_groundAI : public ScriptedAI
{
     mob_freya_groundAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiNatureBomb_Timer;
    uint32 m_uiDieTimer;
    uint32 m_uiEonarsGift_Timer;
    uint32 m_uiNonSelectable_Timer;
    uint32 m_uiGrow_Timer;
    uint32 m_uiSunBeamDespawn_Timer;
    uint32 m_uiUnstableEnergy_Timer;
    uint32 m_uiHealthyGrow_Timer;
    //uint32 m_uiHealthySpamTimer;
    uint64 m_uiNatureBombGUID;
    float m_fSize;

    bool m_bNpcNatureBomb;
    bool m_bNpcEonarsGift;
    bool m_bNpcHealthySpore;
    bool m_bNpcSunBeamFreya;
    bool m_bNpcSunBeamBright;

    bool m_bHasGrown;

    void Reset()
    {
        m_uiNatureBomb_Timer        = urand(9000,11000);
        m_uiDieTimer                = 60000;
        m_uiEonarsGift_Timer        = urand(11000,13000);
        m_uiNonSelectable_Timer     = 5000;
        m_uiUnstableEnergy_Timer    = 1000;
        m_uiGrow_Timer              = 0;
        //m_uiHealthySpamTimer        = 0;
        m_uiNatureBombGUID          = 0;
        m_uiSunBeamDespawn_Timer    = urand(10000,11000);
        m_bHasGrown                 = false;
        m_uiHealthyGrow_Timer       = 2000;
        m_bNpcNatureBomb            = false;
        m_bNpcEonarsGift            = false;
        m_bNpcHealthySpore          = false;
        m_bNpcSunBeamFreya          = false;
        m_bNpcSunBeamBright         = false;

        // the invisible displayIds should be set in DB.
        switch(m_creature->GetEntry())
        {
            case NPC_NATURE_BOMB:
                m_bNpcNatureBomb = true;
                m_creature->setFaction(14);
                m_fSize = 1;
                if (GameObject* pBomb = m_creature->SummonGameobject(GO_NATURE_BOMB, m_creature->GetPositionX(),
                    m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, 0))
                    m_uiNatureBombGUID = pBomb->GetGUID();
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                break;
            case NPC_EONARS_GIFT:
                m_bNpcEonarsGift = true;
                m_fSize = float(0.1);
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                break;
            case NPC_HEALTHY_SPORE:
                m_bNpcHealthySpore = true; 
                DoCast(m_creature, SPELL_HEALTHY_SPORE_VISUAL);
                //m_uiHealthySpamTimer = 500;
                break;
            case NPC_SUN_BEAM:
                m_bNpcSunBeamFreya = true;
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY_FREYA : SPELL_UNSTABLE_ENERGY_FREYA_H);
                break;
            case NPC_UNSTABLE_SUN_BEAM:
                m_bNpcSunBeamBright = true; 
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                //DoCast(m_creature, SPELL_PHOTOSYNTHESIS); // spell needs core fix
                break;
        }

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetRespawnDelay(DAY);           
    }

    void AttackStart(Unit* /*pWho*/)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->isAlive())
            return;

        // NATURE BOMB
        if (m_bNpcNatureBomb)
        {
            if (m_uiNatureBomb_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_NATURE_BOMB : SPELL_NATURE_BOMB_H);
                //m_pInstance->DoUseDoorOrButton(m_uiNatureBombGUID);
                m_uiDieTimer = 500;
                m_uiNatureBomb_Timer = 10000;
            }else m_uiNatureBomb_Timer -= uiDiff;

            if (m_uiDieTimer < uiDiff)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
                if (m_uiNatureBombGUID && m_pInstance)
                    if (GameObject* pBomb = m_pInstance->instance->GetGameObject(m_uiNatureBombGUID))
                        pBomb->Delete();
            }
            else m_uiDieTimer -= uiDiff;
        }

        // EONAR GIFT
        if (m_bNpcEonarsGift)
        {
            if (m_uiGrow_Timer > 500 && m_fSize < 1.5)
            {
                m_fSize += float(m_uiGrow_Timer)/8000;
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fSize);
                m_uiGrow_Timer = 0;
            }else m_uiGrow_Timer += uiDiff;

            if (m_uiEonarsGift_Timer < uiDiff)
            {
                if (Creature* pFreya = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_FREYA))))
                    DoCast(pFreya, m_bIsRegularMode ? SPELL_LIFEBINDERS_GIFT : SPELL_LIFEBINDERS_GIFT_H);
                m_uiEonarsGift_Timer = 1000;
            }else m_uiEonarsGift_Timer -= uiDiff;

            if (m_uiNonSelectable_Timer < uiDiff && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                DoCast(m_creature, SPELL_PHEROMONES_LG);
            }else m_uiNonSelectable_Timer -= uiDiff;
        }

        // HEALTHY SPORE
        if (m_bNpcHealthySpore)
        {
            if (m_bHasGrown && m_fSize < 0.5)
                m_creature->ForcedDespawn();

            //if (!m_creature->HasAura(SPELL_HEALTHY_SPORE_VISUAL))
            //    m_creature->CastSpell(m_creature, SPELL_HEALTHY_SPORE_VISUAL, true);

            //if (m_fSize > 1)
            //{
            //    if (HandleTimer(m_uiHealthySpamTimer, uiDiff, true))
            //    {
            //        m_creature->CastSpell(m_creature, SPELL_POTENT_PHEROMONES_PROC, true);
            //        m_uiHealthySpamTimer = 500;
            //    }
            //}

            if (m_uiHealthyGrow_Timer < uiDiff)
            {
                if (!m_bHasGrown)
                {
                    m_fSize = float(urand(150,225))/100;
                    m_bHasGrown = true;
                }
                else
                    m_fSize = float(urand(1,300))/100;

                m_creature->AddAndLinkAura(SPELL_POTENT_PHEROMONES, m_fSize > 1);
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fSize);
                m_uiHealthyGrow_Timer = 3000;
            }else m_uiHealthyGrow_Timer -= uiDiff;
        }

        // SUN BEAM
        if (m_bNpcSunBeamBright)
        {
            if (m_uiUnstableEnergy_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY : SPELL_UNSTABLE_ENERGY_H);
                m_uiUnstableEnergy_Timer = 1000;
            }else m_uiUnstableEnergy_Timer -= uiDiff;
        }

        if (m_bNpcSunBeamFreya || m_bNpcSunBeamBright)
        {
            if (m_uiSunBeamDespawn_Timer < uiDiff)
                m_creature->ForcedDespawn();
            else m_uiSunBeamDespawn_Timer -= uiDiff;
        }
    }
};

// Script for Freya's adds
struct MANGOS_DLL_DECL mob_freya_spawnedAI : public ScriptedAI
{
    mob_freya_spawnedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bAncientConservator;
    bool m_bDetonatingLasher;
    bool m_bAncientWaterSpirit;
    bool m_bStormLasher;
    bool m_bSnaplasher;
    bool m_bHasExploded;

    uint32 m_uiDeathCountdown;
    uint32 m_uiTidalWave_Timer;
    uint32 m_uiStormbolt_Timer;
    uint32 m_uiLightningLash_Timer;
    uint32 m_uiFlameLash_Timer;
    uint32 m_uiNaturesFury_Timer;
    uint32 m_uiWave3_DeathCountdown;
    uint32 m_uiRespawnSpores_Timer;
    uint32 m_uiDieTimer;
    uint8 m_uiHealthMultiplier;

    void Reset()
    {
        m_bAncientWaterSpirit       = false;
        m_bStormLasher              = false;
        m_bSnaplasher               = false;
        m_bAncientConservator       = false;
        m_bDetonatingLasher         = false;
        m_bHasExploded              = false;
        m_uiDieTimer                = 120000;
        m_uiDeathCountdown          = 10000;
        m_uiTidalWave_Timer         = urand(2000,4000);
        m_uiStormbolt_Timer         = 1000;
        m_uiLightningLash_Timer     = urand(11000,14000);        
        m_uiFlameLash_Timer         = urand(5000,10000);
        m_uiNaturesFury_Timer       = urand(8000,10000);
        m_uiRespawnSpores_Timer     = 5000;
        m_uiHealthMultiplier        = 1;

        switch(m_creature->GetEntry())
        {
            // The Conservator's Grip needs core fix. It should be canceled by pheronomes!
        case NPC_ANCIENT_CONSERVATOR:
            m_bAncientConservator = true;
            DoCast(m_creature, SPELL_CONSERVATORS_GRIP); //spell disabled because it isn't negated by pheronomes
            DoSpores(4);
            break;
        case NPC_DETONATING_LASHER:
            m_bDetonatingLasher = true;
            break;
        case NPC_WATER_SPIRIT:
            m_bAncientWaterSpirit = true;
            break;
        case NPC_SNAPLASHER:
            m_bSnaplasher = true;
            DoCast(m_creature, m_bIsRegularMode ? SPELL_HARDENED_BARK : SPELL_HARDENED_BARK_H);
            break;
        case NPC_STORM_LASHER:
            m_bStormLasher = true;
            break;
        }
        m_creature->SetRespawnDelay(DAY);
    }

    void JustDied(Unit* /*Killer*/)
    {
        // remove some stacks from Freya's aura
        // hacky way. Should be done by spell which needs core support
        if (m_bAncientConservator)
        {
            if (Creature* pFreya = m_pInstance->GetCreature(NPC_FREYA))
            {
                if (Aura* natureAura = pFreya->GetAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0))
                {
                    if (natureAura->modStackAmount(-25))
                    {
                        ((boss_freyaAI*)pFreya->AI())->m_bPhase2 = true;
                        m_creature->RemoveAurasDueToSpell(SPELL_ATTUNED_TO_NATURE);
                    }
                }
            }
            CreatureList list;
            GetCreatureListWithEntryInGrid(list, m_creature, NPC_HEALTHY_SPORE, 200.f);
            for(CreatureList::iterator itr = list.begin(); itr != list.end(); ++itr)
            {
                (*itr)->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.1);
                (*itr)->ForcedDespawn();
            }
        }

        if (m_bDetonatingLasher)
        {
            if (Creature* pFreya = m_pInstance->GetCreature(NPC_FREYA))
            {
                if (Aura* natureAura = pFreya->GetAura(SPELL_ATTUNED_TO_NATURE, EFFECT_INDEX_0))
                {
                    if (natureAura->modStackAmount(-2))
                    {
                        ((boss_freyaAI*)pFreya->AI())->m_bPhase2 = true;
                        m_creature->RemoveAurasDueToSpell(SPELL_ATTUNED_TO_NATURE);
                    }
                }
            }
        }
    }

    void DamageTaken(Unit* /*done_by*/, uint32 &uiDamage)
    {
        if (m_bDetonatingLasher && uiDamage > m_creature->GetHealth() && !m_bHasExploded)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_DETONATE : SPELL_DETONATE_H);
            uiDamage        = 0;
            m_bHasExploded  = true;
            m_uiDieTimer  = 500;
        }        
    }

    void DoSpores(int8 times)
    {
        for(int8 count = 0; count < times; ++count)
        {
            float angle = rand_norm_f()*M_PI_F*2;
            for(int8 i = 0; i < 4; ++i)
            {
                angle += M_PI_F/2;

                float radius = urand(15, 40);
                Coords coord(x,y,m_creature->GetPositionZ()+15.f);
                m_creature->GetNearPoint2D(coord.x, coord.y, radius, angle);
                m_creature->UpdateGroundPositionZ(coord.x, coord.y, coord.z, 30.f);
                m_creature->SummonCreature(NPC_HEALTHY_SPORE, coord, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_FREYA) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->isAlive())
            return;

        // DETONATING LASHERS
        if (m_bDetonatingLasher)
        {
            if (m_uiFlameLash_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_FLAME_LASH);
                m_uiFlameLash_Timer = urand(5000,10000);
            }else m_uiFlameLash_Timer -= uiDiff;

            if (m_uiDieTimer < uiDiff)
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
            else m_uiDieTimer -= uiDiff;
        }

        // CONSERVATOR
        if (m_bAncientConservator)
        {
            if (m_uiNaturesFury_Timer < uiDiff)
            {
                DoCast(m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0), m_bIsRegularMode ? SPELL_NATURES_FURY : SPELL_NATURES_FURY_H);
                m_uiNaturesFury_Timer = urand(5000,6000);
            }else m_uiNaturesFury_Timer -= uiDiff;

            if (m_uiRespawnSpores_Timer < uiDiff)
            {
                DoSpores(1);
                m_uiRespawnSpores_Timer = 5000;
            }else m_uiRespawnSpores_Timer -= uiDiff;
        }

        // ELEMENTAL ADDS
        // waterspirit
        if (m_bAncientWaterSpirit && m_uiTidalWave_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_TIDAL_WAVE : SPELL_TIDAL_WAVE_H);
            m_uiTidalWave_Timer = urand(7000,9000);
        }else m_uiTidalWave_Timer -= uiDiff;

        // stormlasher
        if (m_bStormLasher)
        {
            if (m_uiLightningLash_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_LIGHTNING_LASH : SPELL_LIGHTNING_LASH_H);
                m_uiLightningLash_Timer = urand(11000,14000);
            }
            else
            {
                m_uiLightningLash_Timer -= uiDiff;
                if (m_uiStormbolt_Timer < uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_STORMBOLT : SPELL_STORMBOLT_H);
                    m_uiStormbolt_Timer = 2000;
                }else m_uiStormbolt_Timer -= uiDiff;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_freya_ground(Creature* pCreature)
{
    return new mob_freya_groundAI(pCreature);
}

CreatureAI* GetAI_mob_freya_spawned(Creature* pCreature)
{
    return new mob_freya_spawnedAI(pCreature);
}

CreatureAI* GetAI_mob_iron_roots(Creature* pCreature)
{
    return new mob_iron_rootsAI(pCreature);
}

void AddSC_boss_freya()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_freya";
    newscript->GetAI = &GetAI_boss_freya;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_brightleaf";
    newscript->GetAI = &GetAI_boss_elder_brightleaf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_ironbranch";
    newscript->GetAI = &GetAI_boss_elder_ironbranch;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_stonebark";
    newscript->GetAI = &GetAI_boss_elder_stonebark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freya_ground";
    newscript->GetAI = &GetAI_mob_freya_ground;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freya_spawned";
    newscript->GetAI = &GetAI_mob_freya_spawned;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_iron_roots";
    newscript->GetAI = &GetAI_mob_iron_roots;
    newscript->RegisterSelf();
}
