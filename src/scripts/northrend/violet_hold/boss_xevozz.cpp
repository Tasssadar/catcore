#include "precompiled.h"
#include "violethold.h"
#include "escort_ai.h"

//koule radsi predelat na EscortAI, az bude cas

static float XevozzOutWP[2][3]=
{
    {1900.01f, 831.79f, 38.7323f},
    {1899.84f, 831.53f, 38.7323f},
};

enum
{
    SAY_AGGRO                                 = -1608027,
    SAY_SLAY_1                                = -1608028,
    SAY_SLAY_2                                = -1608029,
    SAY_SLAY_3                                = -1608030,
    SAY_DEATH                                 = -1608031,
    SAY_SPAWN                                 = -1608032,
    SAY_CHARGED                               = -1608033,
    SAY_REPEAT_SUMMON_1                       = -1608034,
    SAY_REPEAT_SUMMON_2                       = -1608035,
    SAY_SUMMON_ENERGY                         = -1608036,

    SPELL_ARCANE_BARRAGE_VOLLEY               = 54202,
    SPELL_ARCANE_BARRAGE_VOLLEY_H             = 59483,
    SPELL_ARCANE_BUFFET                       = 54226,
    SPELL_ARCANE_BUFFET_H                     = 59485,
    SPELL_SUMMON_ETHEREAL_SPHERE_1            = 54102,
    SPELL_SUMMON_ETHEREAL_SPHERE_2            = 54137,
    SPELL_SUMMON_ETHEREAL_SPHERE_3            = 54138,

    NPC_ETHEREAL_SPHERE                       = 29271,
    //NPC_ETHEREAL_SPHERE2                      = 32582, // heroic only?
    SPELL_ARCANE_POWER                        = 54160,
    SPELL_ARCANE_POWER_H                      = 59474,
    SPELL_SUMMON_PLAYERS                      = 54164,
};

struct MANGOS_DLL_DECL boss_xevozzAI : public npc_escortAI
{
   boss_xevozzAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance* m_instance;

    bool m_bIsRegularMode;

    uint32 m_uiSummonEtherealSphere_Timer;
    uint32 m_uiArcaneBarrageVolley_Timer;
    uint32 m_uiArcaneBuffet_Timer;

    uint16 BossStartTimer;

    void Reset()
    {
        m_uiSummonEtherealSphere_Timer = urand(10000, 12000);
        m_uiArcaneBarrageVolley_Timer = urand(20000, 22000);
        m_uiArcaneBuffet_Timer = m_uiSummonEtherealSphere_Timer + urand(5000, 6000);
        BossStartTimer=0;
        DespawnSphere();
    }

   void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void DespawnSphere()
    {
        CreatureList assistList;
        GetCreatureListWithEntryInGrid(assistList,m_creature, NPC_ETHEREAL_SPHERE ,150.0f);

        if (assistList.empty())
            return;
        for(CreatureList::iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
            (*iter)->DealDamage((*iter), (*iter)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetSpeedRate(MOVE_RUN, 0.5f);
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            pSummoned->AddThreat(pTarget);
            pSummoned->AI()->AttackStart(pTarget);
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

    void DoAction (uint32 action)
    {
        switch (action)
        {
        case BOSS_PULL:
            for (uint8 i=0;i<2;i++)
            {
                if (!i)
                    AddWaypoint(i,XevozzOutWP[i][0],XevozzOutWP[i][1],XevozzOutWP[i][2],3000);
                else
                    AddWaypoint(i,XevozzOutWP[i][0],XevozzOutWP[i][1],XevozzOutWP[i][2]);
            }
            SetDespawnAtEnd(false);
            BossStartTimer=5000;
            DoScriptText(SAY_SPAWN,m_creature);
            break;
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

        if (m_uiArcaneBarrageVolley_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE_VOLLEY_H : SPELL_ARCANE_BARRAGE_VOLLEY);
            m_uiArcaneBarrageVolley_Timer = urand(20000, 22000);
        }
        else m_uiArcaneBarrageVolley_Timer -= uiDiff;

        if (m_uiArcaneBuffet_Timer)
            if (m_uiArcaneBuffet_Timer < uiDiff)
            {
               DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_BUFFET_H : SPELL_ARCANE_BUFFET);
                m_uiArcaneBuffet_Timer = 0;
            }
            else m_uiArcaneBuffet_Timer -= uiDiff;

        if (m_uiSummonEtherealSphere_Timer < uiDiff)
        {
            DoScriptText(SAY_SUMMON_ENERGY, m_creature);
            DoCast(m_creature, SPELL_SUMMON_ETHEREAL_SPHERE_1);
            if (!m_bIsRegularMode) // extra one for heroic
                m_creature->SummonCreature(NPC_ETHEREAL_SPHERE, m_creature->GetPositionX()-5+rand()%10, m_creature->GetPositionY()-5+rand()%10, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 40000);

            m_uiSummonEtherealSphere_Timer = urand(45000, 47000);
            m_uiArcaneBuffet_Timer = urand(5000, 6000);
        }
        else m_uiSummonEtherealSphere_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnSphere();
        
        if (m_instance)
        {
            if (!m_bIsRegularMode)
                m_instance->SetData(DATA_XEVOZZ,SPECIAL);
            else
                m_instance->SetData(DATA_XEVOZZ,DONE);
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

struct MANGOS_DLL_DECL mob_ethereal_sphereAI : public ScriptedAI
{
    mob_ethereal_sphereAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance *m_instance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonPlayers_Timer;
    uint32 m_uiRangeCheck_Timer;

    void Reset()
    {
        m_uiSummonPlayers_Timer = urand(33000, 35000);
        m_uiRangeCheck_Timer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (m_instance)
            {
                if (Creature* pXevozz = ((Creature*)Unit::GetUnit((*m_creature), m_instance->GetData64(DATA_XEVOZZ))))
                {
                    float fDistance = m_creature->GetDistance2d(pXevozz);
                    if (fDistance <= 3)
                        DoCast(pXevozz, m_bIsRegularMode ? SPELL_ARCANE_POWER_H : SPELL_ARCANE_POWER);
                    else
                        DoCast(m_creature, 35845);
                }
            }
            m_uiRangeCheck_Timer = 1000;
        }
        else m_uiRangeCheck_Timer -= uiDiff;

        if (m_uiSummonPlayers_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_SUMMON_PLAYERS); // not working right

            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();

                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive())
                            DoTeleportPlayer(i->getSource(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+1, i->getSource()->GetOrientation());//+1 = preventing falling under texture
            }

            m_uiSummonPlayers_Timer = urand(33000, 35000);
        }
        else m_uiSummonPlayers_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_xevozz(Creature* pCreature)
{
    return new boss_xevozzAI (pCreature);
}

CreatureAI* GetAI_mob_ethereal_sphere(Creature* pCreature)
{
    return new mob_ethereal_sphereAI (pCreature);
}

void AddSC_boss_xevozz()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_xevozz";
    newscript->GetAI = &GetAI_boss_xevozz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ethereal_sphere";
    newscript->GetAI = &GetAI_mob_ethereal_sphere;
    newscript->RegisterSelf();
}