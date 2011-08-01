#include "precompiled.h"
#include "escort_ai.h"
#include "violethold.h"

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations PortalLoc[]=
{
    {1857.125f, 763.295f, 38.654f},
    {1925.480f, 849.981f, 47.174f},
    {1892.737f, 744.589f, 47.666f},
    {1878.198f, 850.005f, 43.333f},
    {1909.381f, 806.796f, 38.645f},
    {1936.101f, 802.950f, 52.417f},
};

enum
{
    SAY_AGGRO                                 = -1608018,
    SAY_SLAY_1                                = -1608019,
    SAY_SLAY_2                                = -1608020,
    SAY_SLAY_3                                = -1608021,
    SAY_DEATH                                 = -1608022,
    SAY_SPAWN                                 = -1608023,
    SAY_ENRAGE                                = -1608024,
    SAY_SHATTER                               = -1608025,
    SAY_BUBBLE                                = -1608026,

    SPELL_DRAINED                             = 59820,
    SPELL_FRENZY                              = 54312,
    SPELL_FRENZY_H                            = 59522,
    SPELL_PROTECTIVE_BUBBLE                   = 54306,
    SPELL_WATER_BLAST                         = 54237,
    SPELL_WATER_BLAST_H                       = 59520,
    SPELL_WATER_BOLT_VOLLEY                   = 54241,
    SPELL_WATER_BOLT_VOLLEY_H                 = 59521,

    NPC_ICHOR_GLOBULE                         = 29321,
    SPELL_SPLASH                              = 59516,
};

const float IchoronOutWP[2][3]=
{
    {1908.77f, 785.644f, 37.4466f},
    {1908.80f, 785.644f, 37.4466f}
};

struct MANGOS_DLL_DECL boss_ichoronAI : public npc_escortAI
{
    boss_ichoronAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance *m_instance;
    std::list<uint64> m_lWaterElementsGUIDList;

    bool m_bIsRegularMode;
    bool m_bIsExploded;
    bool m_bIsFrenzy;

    uint32 m_uiBuubleChecker_Timer;
    uint32 m_uiWaterBoltVolley_Timer;
    uint16 BossStartTimer;

    void Reset()
    {
        m_bIsExploded = false;
        m_bIsFrenzy = false;
        m_uiBuubleChecker_Timer = 1000;
        m_uiWaterBoltVolley_Timer = urand(10000, 15000);
        BossStartTimer=0;
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DespawnWaterElements();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCast(m_creature,SPELL_PROTECTIVE_BUBBLE);
        DoScriptText(SAY_BUBBLE,m_creature);
    }

    void WaterElementHit()
    {
        if ((m_creature->GetHealth() + m_creature->GetMaxHealth() * 0.03) > m_creature->GetMaxHealth())
            m_creature->SetHealth(m_creature->GetMaxHealth());
        else
            m_creature->SetHealth(m_creature->GetHealth() + m_creature->GetMaxHealth() * 0.03);
        if (m_bIsExploded)
        {
            DoCast(m_creature, SPELL_PROTECTIVE_BUBBLE);
            DoScriptText(SAY_BUBBLE,m_creature);
            m_bIsExploded = false;
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->RemoveAurasDueToSpell(SPELL_DRAINED);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetSpeedRate(MOVE_RUN, 0.2f);
        pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
        m_lWaterElementsGUIDList.push_back(pSummoned->GetGUID());
    }

    void DespawnWaterElements()
    {
        if (m_lWaterElementsGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lWaterElementsGUIDList.begin(); itr != m_lWaterElementsGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_lWaterElementsGUIDList.clear();
    }

    void DoAction(uint32 action)
    {
        switch (action)
        {
        case BOSS_PULL:
            DoScriptText(SAY_SPAWN,m_creature);
            for (uint8 i=0;i<2;i++)
            {
                if (!i)
                    AddWaypoint(i,IchoronOutWP[i][0],IchoronOutWP[i][1],IchoronOutWP[i][2],3000);
                else
                    AddWaypoint(i,IchoronOutWP[i][0],IchoronOutWP[i][1],IchoronOutWP[i][2]);
            }
            SetDespawnAtEnd(false);
            BossStartTimer=5000;
        }
    }

    void WaypointReached(uint32 point)
    {

        switch(point)
        {
        case 1:
            if (m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
            if (m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
            if (m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_PASSIVE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_PASSIVE);
            if (m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->setFaction(FACTION_VIOLET_HOLD_INVADER);

        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (BossStartTimer)
        {
            if (BossStartTimer<=uiDiff)
            {
                Start();
                BossStartTimer=0;
            }
            else
                BossStartTimer-=uiDiff;
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsFrenzy)
        {
            if (m_uiBuubleChecker_Timer < uiDiff)
            {
                if (!m_bIsExploded)
                {
                    if (!m_creature->HasAura(SPELL_PROTECTIVE_BUBBLE))
                    {
                        DoScriptText(SAY_SHATTER,m_creature);
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_WATER_BLAST_H : SPELL_WATER_BLAST);
                        DoCast(m_creature, SPELL_DRAINED);
                        m_creature->SetHealth(m_creature->GetHealth() - m_creature->GetMaxHealth()*0.3);
                        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                        m_bIsExploded = true;
                     
                        for(uint8 i = 0; i < 10; i++)
                        {
                            int tmp = i/2;
                            m_creature->SummonCreature(NPC_ICHOR_GLOBULE, PortalLoc[tmp].x, PortalLoc[tmp].y, PortalLoc[tmp].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        }
                    }
                    m_uiBuubleChecker_Timer = 3000;
                }
                else
                {
                    bool bIsWaterElementsAlive = false;
                    if (!m_lWaterElementsGUIDList.empty())
                    {
                        for(std::list<uint64>::iterator itr = m_lWaterElementsGUIDList.begin(); itr != m_lWaterElementsGUIDList.end(); ++itr)
                            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                                if (pTemp->isAlive())
                                    bIsWaterElementsAlive = true;
                    }
                    if (!bIsWaterElementsAlive || !m_creature->HasAura(SPELL_DRAINED))
                    {
                        DoCast(m_creature, SPELL_PROTECTIVE_BUBBLE);
                        DoScriptText(SAY_BUBBLE,m_creature);
                        m_bIsExploded = false;
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->RemoveAurasDueToSpell(SPELL_DRAINED);
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    }
                    m_uiBuubleChecker_Timer = 1000;
                }
            }
            else m_uiBuubleChecker_Timer -= uiDiff;
        }

        if (!m_bIsExploded)
        {
            if (m_uiWaterBoltVolley_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_WATER_BOLT_VOLLEY_H : SPELL_WATER_BOLT_VOLLEY);
                m_uiWaterBoltVolley_Timer = urand(10000, 15000);
            }
            else m_uiWaterBoltVolley_Timer -= uiDiff;

            if (!m_bIsFrenzy && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 30)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_FRENZY_H : SPELL_FRENZY);
                m_bIsFrenzy = true;
            }

            DoMeleeAttackIfReady();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnWaterElements();

        if (m_instance)
        {
            if (!m_bIsRegularMode)
                m_instance->SetData(DATA_ICHORON,SPECIAL);
            else
                m_instance->SetData(DATA_ICHORON,DONE);
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID)))
                pCreature->AI()->DoAction(BOSS_DEAD);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
        }
    }
};


struct MANGOS_DLL_DECL mob_ichor_globuleAI : public ScriptedAI
{
    mob_ichor_globuleAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	Reset();
    }
    ScriptedInstance *m_pInstance;

    uint32 m_uiRangeCheck_Timer;
    bool ShouldBeDead;

    void Reset()
    {
        ShouldBeDead=false;
        m_uiRangeCheck_Timer = 1000;
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (ShouldBeDead)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pIchoron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_ICHORON))))
                {
                    float fDistance = m_creature->GetDistance2d(pIchoron);
                    if (fDistance <= 2)
                    {
                        DoCast(m_creature,SPELL_SPLASH);
                        ((boss_ichoronAI*)pIchoron->AI())->WaterElementHit();
                        ShouldBeDead=true;                        
                    }
                }
            }
            m_uiRangeCheck_Timer = 1000;
        }
        else m_uiRangeCheck_Timer -= uiDiff;
    }

    void DamageTaken(Unit* attacker,uint32 &damage)
    {
        if (damage>=m_creature->GetHealth())
        {
            ShouldBeDead=true;
            damage=0;
            DoCast(m_creature, SPELL_SPLASH);
        }
    }

};

CreatureAI* GetAI_boss_ichoron(Creature* pCreature)
{
    return new boss_ichoronAI (pCreature);
}

CreatureAI* GetAI_mob_ichor_globule(Creature* pCreature)
{
    return new mob_ichor_globuleAI (pCreature);
}

void AddSC_boss_ichoron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ichoron";
    newscript->GetAI = &GetAI_boss_ichoron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ichor_globule";
    newscript->GetAI = &GetAI_mob_ichor_globule;
    newscript->RegisterSelf();
}