/* ScriptData
SDName: General Vezax
SD%Complete: 90
SDComment: TODO: yells
SDAuthor: Killerfrca
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_AGGRO                               = -1603120,
    SAY_SURGE                               = -1603123,
    SAY_HARD                                = -1603126,
    SAY_SLAY1                               = -1603121,
    SAY_SLAY2                               = -1603122,
    SAY_BERSERK                             = -1603125,
    SAY_DEATH                               = -1603124,
    EMOTE_VAPORS                            = -1603366,
    EMOTE_SURGE                             = -1603367,
    EMOTE_ANIMUS                            = -1603368,

    SPELL_SHADOW_CRASH                      = 62660,
    SPELL_AURA_DESPAIR                      = 62692,
    SPELL_MARK_OF_FACELESS                  = 63276,
    SPELL_SEARING_FLAMES                    = 62661,
    SPELL_SURGE_OF_DARKNESS                 = 62662,
    SPELL_SARONITE_BARRIER                  = 63364,
    SPELL_SARONITE_VAPORS                   = 63323,
    SPELL_ENRAGE                            = 26662,

    SPELL_SUMMON_SARONITE_ANIMUS            = 63145,
    SPELL_SUMMON_SARONITE_VAPORS            = 63081,

    SPELL_SARONITE_ANIMUS_FORMATION         = 63319,
    SPELL_PROFOUND_DARKNESS                 = 63420,

    NPC_SARONITE_ANIMUS                     = 33524,
    NPC_SARONITE_VAPOR                      = 33488,

    ACHIEVEMENT_HARD_MODE                   = 3181,
    ACHIEVEMENT_HARD_MODE_H                 = 3188
};

float const m_uiMiddleCoords[3] = {1843.0f, 118.0f, 342.0f};
uint8 const m_uiRadius = 50;

float const m_fLootVezax[4] = {1853.5f,   47.4f, 342.3f, 1.62f};     //vezax hard

struct MANGOS_DLL_DECL boss_generalvezaxAI : public ScriptedAI
{
    boss_generalvezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pAnimus = NULL;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    CreatureList m_pVapors;

    float home[3];
    bool HardMode;
    bool AnimusAlive;
    bool VaporKilled;
    bool PrepareAnimus;
    bool IsCastingSearingFlames;
    bool HasSaroniteBarrier;
    bool IsInterruptable;

    uint32 m_uiSearingFlames_Timer;
    uint32 m_uiShadowCrash_Timer;
    uint32 m_uiMarkOfFaceless_Timer;
    uint32 m_uiSurgeOfDarkness_Timer;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiSummonVapors_Timer;
    uint32 m_uiAnimus_Timer;

    uint32 Vapors_Count;

    Creature* pAnimus;

    void Reset()
    {
        m_uiSearingFlames_Timer = 8*IN_MILLISECONDS;
        m_uiShadowCrash_Timer = 10*IN_MILLISECONDS;
        m_uiMarkOfFaceless_Timer = urand(10,35)*IN_MILLISECONDS;
        m_uiSurgeOfDarkness_Timer = urand(20,40)*IN_MILLISECONDS;
        m_uiEnrage_Timer = 10*MINUTE*IN_MILLISECONDS;
        m_uiSummonVapors_Timer = 10*IN_MILLISECONDS;

        Vapors_Count = 0;

        home[0] = m_creature->GetPositionX();
        home[1] = m_creature->GetPositionY();
        home[2] = m_creature->GetPositionZ();

        if (pAnimus)
            pAnimus->ForcedDespawn();

        pAnimus = NULL;

        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);

        m_pVapors.clear();
        GetCreatureListWithEntryInGrid(m_pVapors, m_creature, NPC_SARONITE_VAPOR, 200.0f);
        for(CreatureList::iterator iter = m_pVapors.begin(); iter != m_pVapors.end(); ++iter)
            (*iter)->ForcedDespawn();

        m_pVapors.clear();
        HardMode = false;
        VaporKilled = false;
        IsCastingSearingFlames = false;
        HasSaroniteBarrier = false;
        IsInterruptable = false;

        if (m_pInstance->GetData(TYPE_VEZAX) == IN_PROGRESS)
            m_pInstance->SetData(TYPE_VEZAX, FAIL);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(urand(0,1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VEZAX, DONE);
            if (HardMode)
            {
                m_pInstance->SetData(TYPE_VEZAX_HARD, DONE);
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEVEMENT_HARD_MODE : ACHIEVEMENT_HARD_MODE_H);
                m_creature->SummonGameobject( m_bIsRegularMode ? LOOT_VEZAX_HARD : LOOT_VEZAX_HARD_H, 
                    m_fLootVezax[0], m_fLootVezax[1], m_fLootVezax[2], m_fLootVezax[3], 604800);
            }
        }

        m_creature->SetHardModeKill(HardMode);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);

        // Mana-regen stop aura
        m_creature->CastSpell(m_creature, SPELL_AURA_DESPAIR, true);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, FAIL);

        m_creature->RemoveAurasDueToSpell(SPELL_SARONITE_BARRIER);
    }

    void PreActivateHardMode()
    {
        m_creature->CastSpell(m_creature, SPELL_SARONITE_BARRIER, true);
        HasSaroniteBarrier = true;

        GetCreatureListWithEntryInGrid(m_pVapors, m_creature, NPC_SARONITE_VAPOR, 200.0f);

        for(CreatureList::iterator iter = m_pVapors.begin(); iter != m_pVapors.end(); ++iter)
        {
            Creature* pVapor = *iter;
            pVapor->RemoveAllAuras();
            pVapor->DeleteThreatList();
            pVapor->setFaction(35);
            pVapor->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pVapor->SetSpeedRate(MOVE_RUN, 3.5f, true);
            pVapor->GetMotionMaster()->MoveChase(m_creature);
            pVapor->GetMotionMaster()->MovePoint(0, home[0], home[1], home[2]);

        }
        PrepareAnimus = true;
        m_uiAnimus_Timer = 9*IN_MILLISECONDS;
        HardMode = true;
    }
    void ActivateHardMode()
    {
        if (pAnimus = m_creature->SummonCreature(NPC_SARONITE_ANIMUS, home[0], home[1], home[2], 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            pAnimus->AI()->AttackStart(m_creature->getVictim());
        for(CreatureList::iterator iter = m_pVapors.begin(); iter != m_pVapors.end(); ++iter)
            (*iter)->ForcedDespawn();

        DoScriptText(EMOTE_ANIMUS, m_creature);
        DoScriptText(SAY_HARD, m_creature);

        AnimusAlive = true;
    }

    void AnimusDied()
    {
        AnimusAlive = false;
        m_creature->RemoveAura(SPELL_SARONITE_BARRIER, EFFECT_INDEX_0);
        HasSaroniteBarrier = false;
    }

    void DoSpawnVapors()
    {
        float radius = m_uiRadius* rand_norm_f();
        float angle = 2.0f * M_PI_F * rand_norm_f();
        float x = m_uiMiddleCoords[0] + cos(angle) * radius;
        float y = m_uiMiddleCoords[1] + sin(angle) * radius;
        m_creature->SummonCreature(NPC_SARONITE_VAPOR, x, y, m_uiMiddleCoords[2], angle+M_PI_F, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000);
    }

    void HandleVaporKill(){ VaporKilled = true;}
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!HasSaroniteBarrier || !m_bIsRegularMode)
        {
            // Searing Flames and interrupt immunity
            if (m_uiSearingFlames_Timer < diff)
            {
                DoCast(m_creature, SPELL_SEARING_FLAMES);
                m_uiSearingFlames_Timer = urand(6,9)*IN_MILLISECONDS;
                IsCastingSearingFlames = true;
            }
            else m_uiSearingFlames_Timer -= diff;
            
            if (IsCastingSearingFlames)
            {
                if (!m_creature->IsNonMeleeSpellCasted(false))
                    IsCastingSearingFlames = false;
                if (!IsInterruptable)
                {
                    // removing immune to interrupt (is interruptable)
                    m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                    IsInterruptable = true;
                }
            }
            else
            {
                if (IsInterruptable)
                {
                    // setting imunity back (is interruptable)
                    m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    IsInterruptable = false;
                }
            }
        }

        // Summon Vapors
        if (m_uiSummonVapors_Timer < diff && Vapors_Count < 6)
        {
            DoSpawnVapors();
            //DoCast(m_creature, SPELL_SUMMON_SARONITE_VAPORS);
            DoScriptText(EMOTE_VAPORS, m_creature);
            m_uiSummonVapors_Timer = 30*IN_MILLISECONDS;
            Vapors_Count++;
            if (Vapors_Count == 6 && !VaporKilled)
                PreActivateHardMode();
        }else m_uiSummonVapors_Timer -= diff;

        // summoning Animus
        if (m_uiAnimus_Timer < diff && PrepareAnimus)
        {
            ActivateHardMode();
            PrepareAnimus = false;
        }else m_uiAnimus_Timer -= diff;

        // Shadow Crash
        if (m_uiShadowCrash_Timer < diff)
        {
            if (Unit* target = SelectRandomPlayerPreferRanged(m_bIsRegularMode ? 4 : 9, 15))
                DoCast(target, SPELL_SHADOW_CRASH);
            m_uiShadowCrash_Timer = urand(20,25)*IN_MILLISECONDS;
        }else m_uiShadowCrash_Timer -= diff;

        // Mark Of Faceless
        if (m_uiMarkOfFaceless_Timer < diff)
        {
            if (Unit* target = SelectRandomPlayerPreferRanged(m_bIsRegularMode ? 4 : 9, 15))
                DoCast(target, SPELL_MARK_OF_FACELESS);
            m_uiMarkOfFaceless_Timer = urand(10,45)*IN_MILLISECONDS;
        }else m_uiMarkOfFaceless_Timer -= diff;

        // Surge Of Darkness
        if (m_uiSurgeOfDarkness_Timer < diff)
        {
            DoCast(m_creature, SPELL_SURGE_OF_DARKNESS);
            DoScriptText(SAY_SURGE, m_creature);
            DoScriptText(EMOTE_SURGE, m_creature);
            m_uiSurgeOfDarkness_Timer = urand(60,70)*IN_MILLISECONDS;
        }else m_uiSurgeOfDarkness_Timer -= diff;

        // Enrage
        if (m_uiEnrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            DoScriptText(SAY_BERSERK, m_creature);
            m_uiEnrage_Timer = IN_MILLISECONDS;
        }else m_uiEnrage_Timer -= diff;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct MANGOS_DLL_DECL boss_saroniteanimusAI : public ScriptedAI
{
    boss_saroniteanimusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiProfoundDarkness_Timer;

    Creature* pVezax;
    void Reset()
    {
        m_uiProfoundDarkness_Timer = IN_MILLISECONDS;
        m_creature->CastSpell(m_creature, SPELL_SARONITE_ANIMUS_FORMATION, true);
        if (pVezax = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VEZAX)))
            if (pVezax->getVictim())
                AttackStart(pVezax->getVictim());
    }

    void JustDied(Unit* /*victim*/)
    {
        Creature* boss = pVezax ? pVezax : m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VEZAX));
        if (!boss)
        {
            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive())
                            i->getSource()->DealDamage(i->getSource(), i->getSource()->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            return;
        }
        ((boss_generalvezaxAI*)boss->AI())->AnimusDied();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Profound Darkness
        if (m_uiProfoundDarkness_Timer < diff)
        {
            DoCast(m_creature, SPELL_PROFOUND_DARKNESS);
            m_uiProfoundDarkness_Timer = m_bIsRegularMode ? 2500 : 1500;
        }else m_uiProfoundDarkness_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_saronitevaporsAI : public ScriptedAI
{
    npc_saronitevaporsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Creature* pVezax;

    bool m_bIsDead;

    void Reset()
    {
        pVezax = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VEZAX));
        m_bIsDead = false;
    }
    void AttackStart(Unit* /*pWho*/){return;}
    void DamageTaken(Unit* /*pDoneBy*/, uint32 &damage)
    {
        // Mana regen pool
        if (damage >= m_creature->GetHealth())
        {
            damage = 0;
            if (!m_bIsDead)
            {
                m_creature->SetHealth(1);
                m_creature->CastSpell(m_creature, SPELL_SARONITE_VAPORS, true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_bIsDead = true;

                m_creature->StopMoving();
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                m_creature->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                m_creature->addUnitState(UNIT_STAT_DIED);
                m_creature->CombatStop();

                Creature* boss = pVezax ? pVezax : m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VEZAX));
                if (!boss)
                {
                    Map* pMap = m_creature->GetMap();
                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();
                        if (!PlayerList.isEmpty())
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                if (i->getSource()->isAlive())
                                    i->getSource()->DealDamage(i->getSource(), i->getSource()->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                    return;
                }
                ((boss_generalvezaxAI*)boss->AI())->HandleVaporKill();
            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        Creature* boss = pVezax ? pVezax : (Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_VEZAX));
        if (boss)
            ((boss_generalvezaxAI*)boss->AI())->VaporKilled = true;
    }

    void UpdateAI(const uint32 /*diff*/){}
};

CreatureAI* GetAI_boss_generalvezax(Creature* pCreature)
{
    return new boss_generalvezaxAI(pCreature);
}
CreatureAI* GetAI_boss_saroniteanimus(Creature* pCreature)
{
    return new boss_saroniteanimusAI(pCreature);
}
CreatureAI* GetAI_npc_saronitevapors(Creature* pCreature)
{
    return new npc_saronitevaporsAI(pCreature);
}

void AddSC_boss_vezax()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_generalvezax";
    newscript->GetAI = &GetAI_boss_generalvezax;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_saroniteanimus";
    newscript->GetAI = &GetAI_boss_saroniteanimus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_saronitevapors";
    newscript->GetAI = &GetAI_npc_saronitevapors;
    newscript->RegisterSelf();
}
