#include "precompiled.h"
#include "gundrak.h"

#define IN_MILISECONDS 1000

enum Spells
{
    SPELL_ECK_BERSERK                             = 55816, //Eck goes berserk, increasing his attack speed by 150% and all damage he deals by 500%.
    SPELL_ECK_BITE                                = 55813, //Eck bites down hard, inflicting 150% of his normal damage to an enemy.
    SPELL_ECK_SPIT                                = 55814, //Eck spits toxic bile at enemies in a cone in front of him, inflicting 2970 Nature damage and draining 220 mana every 1 sec for 3 sec.
    SPELL_ECK_SPRING_1                            = 55815, //Eck leaps at a distant target.  --> Drops aggro and charges a random player. Tank can simply taunt him back.
    SPELL_ECK_SPRING_2                            = 55837  //Eck leaps at a distant target.
};

static float EckSpawnPoint[4] = {1643.877930f, 936.278015f, 107.204948f, 0.668432f};

struct MANGOS_DLL_DECL boss_eckAI : public ScriptedAI
{
    boss_eckAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 uiBerserkTimer;
    uint32 uiBiteTimer;
    uint32 uiSpitTimer;
    uint32 uiSpringTimer;

    bool bBerserk;

    ScriptedInstance* pInstance;

    void Reset()
    {
        uiBerserkTimer = urand(60*IN_MILISECONDS,90*IN_MILISECONDS); //60-90 secs according to wowwiki
        uiBiteTimer = 5*IN_MILISECONDS;
        uiSpitTimer = 10*IN_MILISECONDS;
        uiSpringTimer = 8*IN_MILISECONDS;

        bBerserk = false;

        if (pInstance)
            pInstance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiBiteTimer <= diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ECK_BITE);
            uiBiteTimer = urand(8*IN_MILISECONDS,12*IN_MILISECONDS);
        } else uiBiteTimer -= diff;

        if (uiSpitTimer <= diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ECK_SPIT);
            uiSpitTimer = urand(6*IN_MILISECONDS,14*IN_MILISECONDS);
        } else uiSpitTimer -= diff;

        if (uiSpringTimer <= diff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1);
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(pTarget, rand()%2 ? SPELL_ECK_SPRING_1 : SPELL_ECK_SPRING_2);
                uiSpringTimer = urand(5*IN_MILISECONDS,10*IN_MILISECONDS);
            }
        } else uiSpringTimer -= diff;

        //Berserk on timer or 20% of health
        if (!bBerserk)
        {
            if (uiBerserkTimer <= diff)
            {
                DoCast(m_creature, SPELL_ECK_BERSERK);
                bBerserk = true;
            }
            else
            {
                uiBerserkTimer -= diff;
                if (m_creature->GetHealth() < (m_creature->GetMaxHealth()*0.2))
                {
                    DoCast(m_creature, SPELL_ECK_BERSERK);
                    bBerserk = true;
                }
            }
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*killer*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, DONE);
    }
};

CreatureAI* GetAI_boss_eck(Creature* pCreature)
{
    return new boss_eckAI (pCreature);
}

struct MANGOS_DLL_DECL npc_ruins_dwellerAI : public ScriptedAI
{
    npc_ruins_dwellerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    void Reset()
    {}

    void JustDied(Unit * /*who*/)
    {
        if (pInstance)
        {
            pInstance->SetData64(DATA_RUIN_DWELLER_DIED,m_creature->GetGUID());
            if (pInstance->GetData(DATA_ALIVE_RUIN_DWELLERS) == 0)
                m_creature->SummonCreature(CREATURE_ECK, EckSpawnPoint[0],EckSpawnPoint[1],EckSpawnPoint[2],EckSpawnPoint[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60*IN_MILISECONDS);
        }
    }
};

CreatureAI* GetAI_npc_ruins_dweller(Creature* pCreature)
{
    return new npc_ruins_dwellerAI (pCreature);
}

void AddSC_boss_eck()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_eck";
    newscript->GetAI = &GetAI_boss_eck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ruins_dweller";
    newscript->GetAI = &GetAI_npc_ruins_dweller;
    newscript->RegisterSelf();
}
