#include "precompiled.h"
#include "violethold.h"

enum
{
    SAY_AGGRO                                 = -1608000,
    SAY_SLAY_1                                = -1608001,
    SAY_SLAY_2                                = -1608002,
    SAY_SLAY_3                                = -1608003,
    SAY_DEATH                                 = -1608004,
    SAY_SPAWN                                 = -1608005,
    SAY_DISRUPTION                            = -1608006,
    SAY_BREATH_ATTACK                         = -1608007,
    SAY_SPECIAL_ATTACK_1                      = -1608008,
    SAY_SPECIAL_ATTACK_2                      = -1608009,

    SPELL_ARCANE_VACUM                        = 58694,
    SPELL_BLIZZARD                            = 58693,
    SPELL_BLIZZARD_H                          = 59369,
    SPELL_MANA_DESTRUCTION                    = 59374,
    SPELL_TAIL_SWEEP                          = 58690,
    SPELL_TAIL_SWEEP_H                        = 59283,
    SPELL_UNCONTROLLABLE_ENERGY               = 58688,
    SPELL_UNCONTROLLABLE_ENERGY_H             = 59281,
    SPELL_CYANIGOSA_TRANSFORM                 = 58668,
};

const Coords jumpPos(1883.90f, 805.69f, 38.402f);

struct MANGOS_DLL_DECL boss_cyanigosaAI : public ScriptedAI
{
    boss_cyanigosaAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    	Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;

    uint32 m_uiTailSweep_Timer;
    uint32 m_uiManaDestruction_Timer;
    uint32 m_uiBlizzard_Timer;
    uint32 m_uiUncontrollableEnergy_Timer;
    uint32 m_uiArcaneVacuum_Timer;
    uint8 RPPhase;
    uint16 RPTimer;

    void Reset()
    {
        m_uiUncontrollableEnergy_Timer = urand(15000, 16000);
        m_uiManaDestruction_Timer = urand(5000, 6000);
        m_uiBlizzard_Timer = urand(20000, 25000);
        m_uiTailSweep_Timer = urand(10000, 11000);
        m_uiArcaneVacuum_Timer = urand(28000, 33000);
        RPPhase=0;
        RPTimer=0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void DoAction(uint32 action)
    {
        if (action==BOSS_PULL)
        {
            RPPhase=0;
            RPTimer=11000;
            DoScriptText(SAY_SPAWN,m_creature);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (RPTimer)
        {
            if (RPTimer<=uiDiff)
            {
                switch (RPPhase)
                {
                case 0:
                {
                    m_creature->TrajMonsterMove(jumpPos.x, JumpPos.y, JumpPos.z, false, 80.f, 1000);

                    //she should jump here, no spell aviable for that, it doesn't work
                    RPPhase++;
                    RPTimer=5000;//10000;
                    break;
                }
                case 1:
                    DoCast(m_creature,58668);
                    RPPhase++;
                    RPTimer=5000;
                    break;
                case 2:
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
            else RPTimer-=uiDiff;
        }
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiUncontrollableEnergy_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_UNCONTROLLABLE_ENERGY_H : SPELL_UNCONTROLLABLE_ENERGY);
            m_uiUncontrollableEnergy_Timer = urand(15000, 16000);
        }
        else
            m_uiUncontrollableEnergy_Timer -= uiDiff;

        if (m_uiManaDestruction_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_MANA_DESTRUCTION);
            m_uiManaDestruction_Timer = urand(8000, 13000);
        }
        else
            m_uiManaDestruction_Timer -= uiDiff;

        if (m_uiBlizzard_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_BLIZZARD_H : SPELL_BLIZZARD);
            m_uiBlizzard_Timer = urand(20000, 25000);
        }
        else
            m_uiBlizzard_Timer -= uiDiff;

        if (m_uiArcaneVacuum_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_ARCANE_VACUM);
            DoResetThreat();
            m_uiArcaneVacuum_Timer = urand(28000, 33000);
        }
        else
            m_uiArcaneVacuum_Timer -= uiDiff;

        if (m_uiTailSweep_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_TAIL_SWEEP_H : SPELL_TAIL_SWEEP);
            m_uiTailSweep_Timer = urand(10000, 11000);
        }
        else
            m_uiTailSweep_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(DATA_COMPLETED,DONE);
        Creature* Door = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DOOR_GUID));
        if (Door)
            Door->AI()->DoAction(CYANIGOSA);
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

CreatureAI* GetAI_boss_cyanigosa(Creature* pCreature)
{
    return new boss_cyanigosaAI (pCreature);
}

void AddSC_boss_cyanigosa()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_cyanigosa";
    newscript->GetAI = &GetAI_boss_cyanigosa;
    newscript->RegisterSelf();
}
