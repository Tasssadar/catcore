#include "precompiled.h"
#include "violethold.h"
#include "escort_ai.h"

#define S_FIREBOLT              54235
#define S_FLAME_BREATH          54282
#define S_LAVA_BURN             54249
#define S_FIREBOLT_H            59468
#define S_FLAME_BREATH_H        59469
#define S_LAVA_BURN_H           54249
#define S_CAUTERIZING_FLAMES    59466

const float LavanthorOutWP[2][3]=
{
    {1856.563f, 766.154f, 38.653f},
    {1856.563f, 766.154f, 38.653f},
};

struct MANGOS_DLL_DECL boss_lavanthorAI : public npc_escortAI
{
    ScriptedInstance* m_instance;

    boss_lavanthorAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
		HeroicMode = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool HeroicMode;

	uint16 BossStartTimer;
    uint16 FireboltTimer;
    uint16 FlameBreathTimer;
    uint16 LavaBurnTimer;
    uint16 CauterizingFlamesTimer;

    void Reset()
    {
        BossStartTimer=0;
        FireboltTimer = urand(4000,7000);
        FlameBreathTimer = urand(7000,11000);
        LavaBurnTimer = urand(11000,15000);
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
                m_instance->SetData(DATA_LAVANTHOR,SPECIAL);
            else
                m_instance->SetData(DATA_LAVANTHOR,DONE);
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID)))
                pCreature->AI()->DoAction(BOSS_DEAD);
        }   
    }

    void DoAction(uint32 action)
    {
        switch (action)
        {
        case BOSS_PULL:
            for (uint8 i=0;i<2;i++)
            {
                if (!i)
                    AddWaypoint(i,LavanthorOutWP[i][0],LavanthorOutWP[i][1],LavanthorOutWP[i][2],3000);
                else
                    AddWaypoint(i,LavanthorOutWP[i][0],LavanthorOutWP[i][1],LavanthorOutWP[i][2]);
            }
            SetDespawnAtEnd(false);
            BossStartTimer=5000;
            break;
        }
    }
        

    void UpdateEscortAI(const uint32 diff)
    {
        if (BossStartTimer)
        {
            if (BossStartTimer<=diff)
            {
                Start();
                BossStartTimer=0;
            }
            else
                BossStartTimer-=diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (FireboltTimer<diff)
        {
            DoCast(m_creature->getVictim(),HeroicMode ? S_FIREBOLT_H : S_FIREBOLT);
            FireboltTimer=urand(10000,14000);
        }
        else FireboltTimer-=diff;

        if (FlameBreathTimer<diff)
        {
            DoCast(m_creature,HeroicMode ? S_FLAME_BREATH_H : S_FLAME_BREATH);
            FlameBreathTimer=urand(16000,22000);
        }
        else FlameBreathTimer-=diff;

        if (LavaBurnTimer<diff)
        {
            DoCast(m_creature->getVictim(),HeroicMode ? S_LAVA_BURN_H : S_LAVA_BURN);
            LavaBurnTimer=urand(30000,40000);
        }
        else LavaBurnTimer-=diff;

        if (HeroicMode)
        {
            if (CauterizingFlamesTimer<diff)
            {
                DoCast(m_creature,S_CAUTERIZING_FLAMES);
                CauterizingFlamesTimer=urand(30000,45000);
            }
            else CauterizingFlamesTimer-=diff;
        }

        DoMeleeAttackIfReady();
       
    }
};

CreatureAI* GetAI_boss_lavanthor(Creature* pCreature)
{
    return new boss_lavanthorAI(pCreature);
}

void AddSC_boss_lavanthor()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_lavanthor";
    newscript->GetAI = &GetAI_boss_lavanthor;
    newscript->RegisterSelf();
}
