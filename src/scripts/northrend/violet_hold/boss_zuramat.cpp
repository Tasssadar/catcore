#include "precompiled.h"
#include "violethold.h"
#include "escort_ai.h"

static float ZuramatOutWP[2][3]=
{
    {1924.97f, 850.68f, 47.176f},
    {1924.38f, 849.91f, 47.171f},
};

enum
{
    SAY_AGGRO                                 = -1608037,
    SAY_SLAY_1                                = -1608038,
    SAY_SLAY_2                                = -1608039,
    SAY_SLAY_3                                = -1608040,
    SAY_DEATH                                 = -1608041,
    SAY_SPAWN                                 = -1608042,
    SAY_SHIELD                                = -1608043,
    SAY_WHISPER                               = -1608044,

    SPELL_SHROUD_OF_DARKNESS                  = 54524,
    SPELL_SHROUD_OF_DARKNESS_H                = 59745,
    SPELL_SUMMON_VOID_SENTRY                  = 54369,
    SPELL_VOID_SHIFT                          = 54361,
    SPELL_VOID_SHIFT_H                        = 59743,

    NPC_VOID_SENTRY                           = 29364,
    SPELL_VOID_SENTRY_AURA                    = 54341,
    SPELL_VOID_SENTRY_AURA_H                  = 54351,
    SPELL_SHADOW_BOLT_VOLLEY                  = 54358, // 54342? 54358?
    SPELL_SHADOW_BOLT_VOLLEY_H                = 59747,
};

struct MANGOS_DLL_DECL boss_zuramatAI : public npc_escortAI
{
    boss_zuramatAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance *m_instance;

    bool m_bIsRegularMode;
    std::list<uint64> m_lSentryGUIDList;

    uint32 m_uiShroudDarkness_Timer;
    uint32 m_uiVoidShift_Timer;
    uint32 m_uiSummonVoidSentry_Timer;

    uint16 BossStartTimer;

    void Reset()
    {
        m_uiShroudDarkness_Timer = urand(8000, 9000);
        m_uiSummonVoidSentry_Timer = urand(5000, 10000);
        m_uiVoidShift_Timer = 10000;
        BossStartTimer=0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_lSentryGUIDList.push_back(pSummoned->GetGUID());
        //pSummoned->AddThreat(m_creature);
        //pSummoned->AI()->AttackStart(m_creature);
    }

    void DespawnSentry()
    {
        if (m_lSentryGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lSentryGUIDList.begin(); itr != m_lSentryGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
            {
                if (pTemp->isAlive())
                    //pTemp->ForcedDespawn();
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_lSentryGUIDList.clear();
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
                    AddWaypoint(i,ZuramatOutWP[i][0],ZuramatOutWP[i][1],ZuramatOutWP[i][2],3000);
                else
                    AddWaypoint(i,ZuramatOutWP[i][0],ZuramatOutWP[i][1],ZuramatOutWP[i][2]);
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

        if (m_uiShroudDarkness_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_SHROUD_OF_DARKNESS_H : SPELL_SHROUD_OF_DARKNESS);
            m_uiShroudDarkness_Timer = urand(7000, 8000);
        }
        else m_uiShroudDarkness_Timer -= uiDiff;

        if (m_uiVoidShift_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_VOID_SHIFT_H : SPELL_VOID_SHIFT);
            m_uiVoidShift_Timer = urand(10000, 11000);
        }
        else m_uiVoidShift_Timer -= uiDiff;

        if (m_uiSummonVoidSentry_Timer < uiDiff)
        {
            m_creature->SummonCreature(NPC_VOID_SENTRY, m_creature->GetPositionX()-10+rand()%20, m_creature->GetPositionY()-10+rand()%20, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_uiSummonVoidSentry_Timer = urand(10000, 11000);
        }
        else m_uiSummonVoidSentry_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnSentry();

        if (m_instance)
        {
            if (!m_bIsRegularMode)
                m_instance->SetData(DATA_ZURAMAT,SPECIAL);
            else
                m_instance->SetData(DATA_ZURAMAT,DONE);
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

struct MANGOS_DLL_DECL mob_zuramat_sentryAI : public ScriptedAI
{
    mob_zuramat_sentryAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
    	Reset();
    }
    ScriptedInstance *m_instance;
    bool m_bIsRegularMode;

    void Reset()
    {
        //DoCast(m_creature, m_bIsRegularMode ? SPELL_VOID_SENTRY_AURA_H : SPELL_VOID_SENTRY_AURA); ??
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT_VOLLEY_H : SPELL_SHADOW_BOLT_VOLLEY);
    }
};

CreatureAI* GetAI_boss_zuramat(Creature* pCreature)
{
    return new boss_zuramatAI (pCreature);
}

CreatureAI* GetAI_mob_zuramat_sentry(Creature* pCreature)
{
    return new mob_zuramat_sentryAI (pCreature);
}

void AddSC_boss_zuramat()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_zuramat";
    newscript->GetAI = &GetAI_boss_zuramat;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zuramat_sentry";
    newscript->GetAI = &GetAI_mob_zuramat_sentry;
    newscript->RegisterSelf();

}