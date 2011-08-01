/* Script Data Start
SDName: Boss gal_darah
SDAuthor: LordVanMartin
SD%Complete:
SDComment:
SDCategory:
Script Data End */

/*** SQL START ***
update creature_template set scriptname = '' where entry = '';
*** SQL END ***/
#include "precompiled.h"
#include "gundrak.h"

//Spells
enum Spells
{
    SPELL_ENRAGE                                  = 55285,
    H_SPELL_ENRAGE                                = 59828,
    SPELL_IMPALING_CHARGE                         = 54956,
    H_SPELL_IMPALING_CHARGE                       = 59827,
    SPELL_STOMP                                   = 55292,
    H_SPELL_STOMP                                 = 59826,
    SPELL_PUNCTURE                                = 55276,
    H_SPELL_PUNCTURE                              = 59826,
    SPELL_STAMPEDE                                = 55218,
    SPELL_WHIRLING_SLASH                          = 55249,
    H_SPELL_WHIRLING_SLASH                        = 55825
};

//Yells
enum Yells
{
    SAY_AGGRO                       = -1604019,
    SAY_TRANSFORM_1                 = -1604020,
    SAY_TRANSFORM_2                 = -1604021,
    SAY_SUMMON_1                    = -1604022,
    SAY_SUMMON_2                    = -1604023,
    SAY_SUMMON_3                    = -1604024,
    SAY_SLAY_1                      = -1604025,
    SAY_SLAY_2                      = -1604026,
    SAY_SLAY_3                      = -1604027,
    SAY_DEATH                       = -1604028
};

enum CombatPhase
{
    TROLL,
    RHINO
};

struct MANGOS_DLL_DECL boss_gal_darahAI : public ScriptedAI
{
    boss_gal_darahAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    uint32 uiStampedeTimer;
    uint32 uiWhirlingSlashTimer;
    uint32 uiPunctureTimer;
    uint32 uiEnrageTimer;
    uint32 uiImpalingChargeTimer;
    uint32 uiStompTimer;

    CombatPhase Phase;

    uint8 uiPhaseCounter;

    ScriptedInstance* pInstance;

    void Reset()
    {
        uiStampedeTimer = 10000;
        uiWhirlingSlashTimer = 20000;
        uiPunctureTimer = 10000;
        uiEnrageTimer = 15000;
        uiImpalingChargeTimer = 20000;
        uiStompTimer = 25000;
        uiPhaseCounter = 0;

        Phase = TROLL;
        m_creature->SetDisplayId(27061);

        if (pInstance)
            pInstance->SetData(DATA_GAL_DARAH_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (pInstance)
            pInstance->SetData(DATA_GAL_DARAH_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (Phase)
        {
            case TROLL:
                if (uiPhaseCounter == 2)
                {
                    //FIX: implement transformation
                    m_creature->SetDisplayId(26265);
                    Phase = RHINO;
                    uiPhaseCounter = 0;
                    DoScriptText(SAY_TRANSFORM_1,m_creature);
                }
                else
                {
                    if (uiStampedeTimer <= diff)
                    {
                        DoCast(m_creature, SPELL_STAMPEDE);
                        int say;
                        switch (urand (0, 2))
                        {
                            case 0:
                                say = SAY_SUMMON_1;
                                break;
                            case 1:
                                say = SAY_SUMMON_2;
                                break;
                            case 2:
                                say = SAY_SUMMON_3;
                                break;
                        }
                        DoScriptText(say, m_creature);
                        uiStampedeTimer = 15000;
                    } else uiStampedeTimer -= diff;

                    if (uiWhirlingSlashTimer <= diff)
                    {
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_WHIRLING_SLASH, H_SPELL_WHIRLING_SLASH));
                        uiWhirlingSlashTimer = 20000;
                        ++uiPhaseCounter;
                    } else uiWhirlingSlashTimer -= diff;
                }
                break;
            case RHINO:
                if (uiPhaseCounter == 2)
                {
                    //FIX: implement transformation
                    m_creature->SetDisplayId(27061);
                    Phase = TROLL;
                    uiPhaseCounter = 0;
                    DoScriptText(SAY_TRANSFORM_2,m_creature);
                }
                else
                {
                    if (uiPunctureTimer <= diff)
                    {
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_PUNCTURE, H_SPELL_PUNCTURE));
                        uiPunctureTimer = 8000;
                    } else uiPunctureTimer -= diff;

                    if (uiEnrageTimer <= diff)
                    {
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_ENRAGE, H_SPELL_ENRAGE));
                        uiEnrageTimer = 20000;
                    } else uiEnrageTimer -= diff;

                    if (uiStompTimer <= diff)
                    {
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_STOMP, H_SPELL_STOMP));
                        uiStompTimer = 20000;
                    } else uiStompTimer -= diff;

                    if (uiImpalingChargeTimer <= diff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                            DoCast(pTarget, HEROIC(SPELL_IMPALING_CHARGE, H_SPELL_IMPALING_CHARGE));
                        uiImpalingChargeTimer = 30000;
                        ++uiPhaseCounter;
                    } else uiImpalingChargeTimer -= diff;
                }
                break;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (pInstance)
            pInstance->SetData(DATA_GAL_DARAH_EVENT, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        if (victim == m_creature)
            return;

        int say;
        switch (urand(0, 2))
        {
            case 0:
                say = SAY_SLAY_1;
                break;
            case 1:
                say = SAY_SLAY_2;
                break;
            case 2:
                say = SAY_SLAY_3;
                break;
        }
        DoScriptText(say, m_creature);
    }
};

CreatureAI* GetAI_boss_gal_darah(Creature* pCreature)
{
    return new boss_gal_darahAI (pCreature);
}

void AddSC_boss_gal_darah()
{
    Script *newscript;

    newscript = new Script();
    newscript->Name = "boss_gal_darah";
    newscript->GetAI = &GetAI_boss_gal_darah;
    newscript->RegisterSelf();
}
