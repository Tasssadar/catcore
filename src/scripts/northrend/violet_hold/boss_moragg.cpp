#include "precompiled.h"
#include "violethold.h"
#include "escort_ai.h"

#define SPELL_CORROSICE_SALIVA                54527
#define SPELL_OPTIC_LINK                      54396
#define SPELL_RAY_PAIN                        59525
#define SPELL_RAY_SUFFERING                   54417

const float MoraggOutWP[2][3]=
{
    {1891.400f, 753.418f, 47.667f},
    {1891.400f, 753.418f, 47.667f},
};

struct MANGOS_DLL_DECL boss_moraggAI : public npc_escortAI
{
    ScriptedInstance* m_instance;

    uint32 m_uiCorrosiveSaliva_Timer;
    uint32 m_uiOpticLink_Timer;
    uint32 m_uiRay_Timer;

    bool HeroicMode;

    boss_moraggAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
        HeroicMode = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint16 BossStartTimer;

    void Reset()
    {
        BossStartTimer=0;
        m_uiCorrosiveSaliva_Timer = urand(10000, 11000);
        m_uiOpticLink_Timer = urand(25000, 30000);
        m_uiRay_Timer = urand(2000, 7000);
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

    void JustDied(Unit* killer)
    {
        if (m_instance)
        {
            if (HeroicMode)
                m_instance->SetData(DATA_MORAGG,SPECIAL);
            else
                m_instance->SetData(DATA_MORAGG,DONE);
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID)))
                pCreature->AI()->DoAction(BOSS_DEAD);
        }   
    }

    void DoAction(uint32 action)
    {
        switch (action)
        {
        case BOSS_PULL:
            //m_creature->MonsterSay("Prej me pullnuli",0,m_creature->GetGUID());
            for (uint8 i=0;i<2;i++)
            {
                if (!i)
                    AddWaypoint(i,MoraggOutWP[i][0],MoraggOutWP[i][1],MoraggOutWP[i][2],3000);
                else
                    AddWaypoint(i,MoraggOutWP[i][0],MoraggOutWP[i][1],MoraggOutWP[i][2]);
            }
            SetDespawnAtEnd(false);
            BossStartTimer=5000;
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

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCorrosiveSaliva_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CORROSICE_SALIVA);
            m_uiCorrosiveSaliva_Timer = urand(10000, 11000);
        }
        else m_uiCorrosiveSaliva_Timer -= uiDiff;

        if (m_uiOpticLink_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_OPTIC_LINK);
            m_uiOpticLink_Timer = urand(25000, 30000);
        }
        else m_uiOpticLink_Timer -= uiDiff;

        if (m_uiRay_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, urand(0, 1) ? SPELL_RAY_PAIN : SPELL_RAY_SUFFERING);
            m_uiRay_Timer = urand(2000, 7000);
        }
        else m_uiRay_Timer -= uiDiff;

        DoMeleeAttackIfReady();               
    }
};

CreatureAI* GetAI_boss_moragg(Creature* pCreature)
{
    return new boss_moraggAI(pCreature);
}

void AddSC_boss_moragg()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_moragg";
    newscript->GetAI = &GetAI_boss_moragg;
    newscript->RegisterSelf();
}
