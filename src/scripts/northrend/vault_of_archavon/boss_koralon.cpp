#include "precompiled.h"

enum{
    // 5% increase dmg every 20s
    SPELL_BURNING_FURY          = 68168,
    // every 30s after combat
    SPELL_METEOR_FISTS_AURA_N   = 66725,
    SPELL_METEOR_FISTS_N        = 66765,
    SPELL_METEOR_FISTS_AURA_H   = 68161,
    SPELL_METEOR_FISTS_H        = 67333,

    SPELL_FLAMING_CINDER_N      = 66684,
    SPELL_FLAMING_CINDER_H      = 67332,

    //INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('66665', '1', '35013');
    SPELL_BURNING_BREATH_N      = 66665,
    //INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('67328', '1', '35360');
    SPELL_BURNING_BREATH_H      = 67328,

    CINDER_TARGETS              = 3
};

struct MANGOS_DLL_DECL boss_koralonAI : public ScriptedAI
{
    boss_koralonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    uint32 MeteorFists_Timer;
    uint32 FlamingCinder_Timer;
    uint32 BurningBreath_Timer;
    uint32 Rotate_Timer;
    uint8 rotateCount;
    bool bRotate;

    void Reset()
    {
        MeteorFists_Timer = 30000;
        FlamingCinder_Timer = 15000;
        BurningBreath_Timer = 12000;
        
        rotateCount = 0;
        bRotate = false;
        Rotate_Timer = 750;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        //if (m_pInstance->GetData(TYPE_BOSS_KORALON) != DONE)
        //    m_pInstance->SetData(TYPE_BOSS_KORALON, IN_PROGRESS);

        DoCast(m_creature,SPELL_BURNING_FURY);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
                return;

        //m_pInstance->SetData(TYPE_BOSS_KORALON, DONE);
    }

    void JustReachedHome()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_BURNING_FURY);
        //m_pInstance->SetData(TYPE_BOSS_KORALON, FAIL);
    }

    void Turn()
    {
        ++rotateCount;
        float angle = m_creature->GetOrientation()+(M_PI_F/2)*rotateCount;
        angle = (angle >= 0) ? angle : 2 * M_PI_F + angle;
        angle = (angle <= 2*M_PI_F) ? angle : angle - 2 * M_PI_F;
        m_creature->SetOrientation(angle);

        if (rotateCount == 1)
        {
            m_creature->addUnitState(UNIT_STAT_IGNORE_TARGET);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_TURN_RIGHT);
            m_creature->GetMap()->AddUpdateObject(m_creature);
        }
        else if (rotateCount == 4)
        {
            rotateCount = 0;
            bRotate = false;
            m_creature->clearUnitState(UNIT_STAT_IGNORE_TARGET);
            m_creature->GetMap()->AddUpdateObject(m_creature);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_TURN_RIGHT);
        }
        
        WorldPacket heart;
        m_creature->BuildHeartBeatMsg(&heart);
        m_creature->SendMessageToSet(&heart, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (FlamingCinder_Timer < diff)
        {
            for(int i = 0; i < CINDER_TARGETS; i++)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_FLAMING_CINDER_N : SPELL_FLAMING_CINDER_H);
            }
            FlamingCinder_Timer = 12000;
        }else FlamingCinder_Timer -= diff;

        if (bRotate)
        {
            if (Rotate_Timer <= diff)
            {
                Turn();
                Rotate_Timer = 750;
            }else Rotate_Timer -= diff;
        }
        
        if (BurningBreath_Timer < diff)
        {
            DoCast(m_creature,m_bIsRegularMode ? SPELL_BURNING_BREATH_N : SPELL_BURNING_BREATH_H);
            BurningBreath_Timer = 45000;
            Rotate_Timer = 750;
            bRotate = true;
            Turn();
        }else BurningBreath_Timer -= diff;

        if (MeteorFists_Timer < diff)
        {
            DoCast(m_creature,m_bIsRegularMode ? SPELL_METEOR_FISTS_AURA_N : SPELL_METEOR_FISTS_AURA_H);
            MeteorFists_Timer = 30000;
        }else MeteorFists_Timer -= diff;

        if (m_creature->HasAura(m_bIsRegularMode ? SPELL_METEOR_FISTS_AURA_N : SPELL_METEOR_FISTS_AURA_H))
        {
            DoCast(m_creature->getVictim(),m_bIsRegularMode ? SPELL_METEOR_FISTS_N : SPELL_METEOR_FISTS_H);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_koralon(Creature* pCreature)
{
    return new boss_koralonAI(pCreature);
}

void AddSC_boss_koralon()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_koralon";
    NewScript->GetAI = &GetAI_boss_koralon;
    NewScript->RegisterSelf();
} 