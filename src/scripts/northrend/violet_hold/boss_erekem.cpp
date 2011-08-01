#include "precompiled.h"
#include "violethold.h"
#include "escort_ai.h"

enum
{
    SAY_AGGRO                                 = -1608010,
    SAY_SLAY_1                                = -1608011,
    SAY_SLAY_2                                = -1608012,
    SAY_SLAY_3                                = -1608013,
    SAY_DEATH                                 = -1608014,
    SAY_SPAWN                                 = -1608015,
    SAY_ADD_KILED                             = -1608016,
    SAY_BOTH_ADDS_KILED                       = -1608017,

    SPELL_BLOODLUST                           = 54516,
    SPELL_BREAK_BONDS                         = 59463,
    SPELL_CHAIN_HEAL                          = 54481,
    SPELL_CHAIN_HEAL_H                        = 59473,
    SPELL_EARTH_SHIELD                        = 54479,
    SPELL_EARTH_SHIELD_H                      = 59471,
    SPELL_EARTH_SHOCK                         = 54511,
    SPELL_LIGHTNING_BOLT                      = 53044,
    SPELL_STORMSTRIKE                         = 51876,

    SPELL_GUSHING_WOUND                       = 39215,
    SPELL_HOWLING_SCREECH                     = 54462,
    SPELL_STRIKE                              = 14516,

    NPC_EREKEM_GUARD       = 29395,
};

const float ErekemOutWP[2][3] = 
{
    {1877.32f, 854.05f, 43.3344f},
    {1877.35f, 854.05f, 43.3344f}
};

const float Guard1WP[2][3] =
{
    {1871.82f, 854.10f, 43.3344f},
    {1871.85f, 854.10f, 43.3344f}
};

const float Guard2WP[2][3] = 
{
    {1881.50f, 854.07f, 43.3344f},
    {1881.52f, 854.07f, 43.3344f}
};

const float GuardsSpawn[3] = {1853.75f, 862.453f, 43.4161f};

struct MANGOS_DLL_DECL boss_erekemAI : public npc_escortAI
{
    boss_erekemAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance *m_instance;

    bool m_bIsRegularMode;
    bool m_bIsAddDead;

    uint16 BossStartTimer;

    uint32 m_uiBloodlust_Timer;
    uint32 m_uiBreakBonds_Timer;
    uint32 m_uiChainHeal_Timer;
    uint32 m_uiEarthShield_Timer;
    uint32 m_uiEarthShock_Timer;
    uint32 m_uiLightningBolt_Timer;
    uint32 m_uiStormstrike_Timer;

    void Reset()
    {
        m_bIsAddDead = false;
        m_uiLightningBolt_Timer = 2000;
        m_uiEarthShield_Timer = urand(15000, 20000);
        m_uiEarthShock_Timer = urand(12000, 17000);
        m_uiChainHeal_Timer = urand(5000, 25000);
        m_uiBreakBonds_Timer = urand(25000, 30000);
        m_uiBloodlust_Timer = urand(60000, 65000);
        m_uiStormstrike_Timer = urand(1000, 2000);

        BossStartTimer=0;

        std::list<Creature*> lUnitList;
        GetCreatureListWithEntryInGrid(lUnitList, m_creature, NPC_EREKEM_GUARD, 100.0f);
        if (!lUnitList.empty())
            for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
                if ((*iter))
                    if ((*iter)->isDead())
                        (*iter)->Respawn();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
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
                    AddWaypoint(i,ErekemOutWP[i][0],ErekemOutWP[i][1],ErekemOutWP[i][2],3000);
                else
                    AddWaypoint(i,ErekemOutWP[i][0],ErekemOutWP[i][1],ErekemOutWP[i][2]);
            }
            SetDespawnAtEnd(false);
            BossStartTimer=5000;
            std::list<Creature*> lUnitList;
            GetCreatureListWithEntryInGrid(lUnitList, m_creature, NPC_EREKEM_GUARD, 100.0f);
            if (!lUnitList.empty())
                for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
                    if ((*iter))
                        (*iter)->AI()->DoAction(BOSS_PULL);
            break;
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

        if (m_uiEarthShield_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EARTH_SHIELD_H : SPELL_EARTH_SHIELD);
            m_uiEarthShield_Timer = urand(15000, 20000);
        }
        else m_uiEarthShield_Timer -= uiDiff;

        if (m_uiEarthShock_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_EARTH_SHOCK);
            m_uiEarthShock_Timer = urand(12000, 17000);
        }
        else m_uiEarthShock_Timer -= uiDiff;

        if (m_uiChainHeal_Timer < uiDiff)
        {
            //m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CHAIN_HEAL_H : SPELL_CHAIN_HEAL);
            m_uiChainHeal_Timer = urand(5000, 25000);
        }
        else m_uiChainHeal_Timer -= uiDiff;

        if (m_uiBreakBonds_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_BREAK_BONDS);
            m_uiBreakBonds_Timer = urand(25000, 30000);
        }
        else m_uiBreakBonds_Timer -= uiDiff;

        if (!m_bIsAddDead)
        {
            if (m_uiLightningBolt_Timer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCast(pTarget, SPELL_LIGHTNING_BOLT);
                m_uiLightningBolt_Timer = 2000;
            }
            else m_uiLightningBolt_Timer -= uiDiff;
        }
        else
        {
            if (m_uiStormstrike_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_STORMSTRIKE);
                m_uiStormstrike_Timer = 1000;
            }
            else m_uiStormstrike_Timer -= uiDiff;

            DoMeleeAttackIfReady();
       }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_instance)
        {
            if (!m_bIsRegularMode)
                m_instance->SetData(DATA_EREKEM,SPECIAL);
            else
                m_instance->SetData(DATA_EREKEM,DONE);
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

struct MANGOS_DLL_DECL mob_erekem_guardAI : public npc_escortAI
{
    mob_erekem_guardAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
    	m_instance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	Reset();
    }
    ScriptedInstance *m_instance;

    uint32 m_uiGushingWound_Timer;
    uint32 m_uiHowlingScreech_Timer;
    uint32 m_uiStrike_Timer;
    uint16 BossStartTimer;

    void Reset()
    {
        m_uiGushingWound_Timer = urand(5000, 10000);
        m_uiHowlingScreech_Timer = urand(12000, 15000);
        m_uiStrike_Timer = urand(10000, 11000);
        BossStartTimer=0;
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

    void DoAction(uint32 action)
    {
        switch(action)
        {
        case BOSS_PULL:
            if (m_creature->GetDistance(GuardsSpawn[0],GuardsSpawn[1],GuardsSpawn[2])<5.0f)
            {
                for (uint8 i=0;i<2;i++)
                {
                    if (!i)
                        AddWaypoint(i,Guard1WP[i][0],Guard1WP[i][1],Guard1WP[i][2],3000);
                    else
                        AddWaypoint(i,Guard1WP[i][0],Guard1WP[i][1],Guard1WP[i][2]);
                }
                SetDespawnAtEnd(false);
                BossStartTimer=5000;
            }
            else
            {
                for (uint8 i=0;i<2;i++)
                {
                    if (!i)
                        AddWaypoint(i,Guard2WP[i][0],Guard2WP[i][1],Guard2WP[i][2],3000);
                    else
                        AddWaypoint(i,Guard2WP[i][0],Guard2WP[i][1],Guard2WP[i][2]);
                }
                SetDespawnAtEnd(false);
                BossStartTimer=5000;
            }
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

        if (m_uiGushingWound_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_GUSHING_WOUND);
            m_uiGushingWound_Timer = urand(30000, 32000);
        }
        else m_uiGushingWound_Timer -= uiDiff;

        if (m_uiHowlingScreech_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_HOWLING_SCREECH);
            m_uiHowlingScreech_Timer = urand(24000, 30000);
        }
        else m_uiHowlingScreech_Timer -= uiDiff;

        if (m_uiStrike_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrike_Timer = urand(15000, 16000);
        }
        else m_uiStrike_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_instance)
            if (Creature* pErekem = ((Creature*)Unit::GetUnit((*m_creature), m_instance->GetData64(DATA_EREKEM))))
                if (pErekem->isAlive())
                {
                    DoScriptText(SAY_ADD_KILED, pErekem);
                    pErekem->InterruptNonMeleeSpells(false);
                    pErekem->CastSpell(pErekem, SPELL_BLOODLUST, false);
                    ((boss_erekemAI*)pErekem->AI())->m_bIsAddDead = true;
                }
    }
};

CreatureAI* GetAI_boss_erekem(Creature* pCreature)
{
    return new boss_erekemAI (pCreature);
}

CreatureAI* GetAI_mob_erekem_guard(Creature* pCreature)
{
    return new mob_erekem_guardAI (pCreature);
}

void AddSC_boss_erekem()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_erekem";
    newscript->GetAI = &GetAI_boss_erekem;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_erekem_guard";
    newscript->GetAI = &GetAI_mob_erekem_guard;
    newscript->RegisterSelf();
}