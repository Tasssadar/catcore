#include "precompiled.h"
#include "gundrak.h"

enum Spells
{
    SPELL_EMERGE        = 54850,
    SPELL_EMERGE_2      = 54851,

    SPELL_MIGHTY_BLOW   = 54719,

    SPELL_MOJO_VOLLEY   = 54849,

    SPELL_MERGE         = 54878,
    SPELL_SURGE         = 54801,

    SPELL_FREEZE_ANIM   = 16245,

    SPELL_MOJO_PUDDLE   = 55627,
    H_SPELL_MOJO_PUDDLE = 58994,

    SPELL_MOJO_WAVE     = 55626,
    H_SPELL_MOJO_WAVE   = 58993
};

enum Entries
{
    DRAKKARI_COLOSSUS   = 29307,
};

enum WayPoints
{
    POINT_DRAKKARI_COLOSSUS = 0
};

enum NPCs
{
    NPC_MOJO    = 29830,
    NPC_DRAKKARI_ELEMENTAL = 29573
};

struct MANGOS_DLL_DECL boss_drakkari_colossusAI : public ScriptedAI
{
    boss_drakkari_colossusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_pElemental = NULL;
        Reset();
    }

    ScriptedInstance* pInstance;
    Creature* m_pElemental;

    bool Summoned;
    bool m_bActive;

    uint32 MightyBlowTimer;
    uint16 SpellFreezeAnim;
    uint8 m_uiMojoCount;

    void Reset()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
        
        if (pInstance)
        {
            // Despawn possible Elemental
            if (m_pElemental)
                m_pElemental->ForcedDespawn();

            RespawnMojos();

            pInstance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, NOT_STARTED);
        }

        Summoned = false;
        m_pElemental = NULL;

        MightyBlowTimer = 10000;
        m_uiMojoCount = 0;

        SpellFreezeAnim=0;

        SetActive(false);
    }

    void AddMojo()
    {
        if (++m_uiMojoCount < 5)
            return;

        SetActive(true);
    }

    void StartEvent()
    {
        std::list<Creature*> mojos;
        GetCreatureListWithEntryInGrid(mojos, m_creature, NPC_MOJO, 20.0f);
        while (!mojos.empty())
        {
            Creature* mojo = mojos.front();
            mojos.pop_front();

            mojo->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            mojo->GetMotionMaster()->MovePoint(POINT_DRAKKARI_COLOSSUS, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
        }
    }

    void RespawnMojos()
    {
        std::list<Creature*> mojos;
        GetCreatureListWithEntryInGrid(mojos, m_creature, NPC_MOJO, 20.0f);
        while (!mojos.empty())
        {
            Creature* mojo = mojos.front();
            mojos.pop_front();
            
            if (mojo->isAlive())
                mojo->AI()->EnterEvadeMode();
            else
                mojo->Respawn();
        }
    }

    void SetActive(bool active)
    {
        if (!active)
        {
            m_creature->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveAllAuras();
            SpellFreezeAnim=3000;
            //m_creature->CastSpell(m_creature, SPELL_FREEZE_ANIM, true);
            m_creature->GetMotionMaster()->MoveIdle();

        }
        else
        {
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            if (Unit* victim = m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(victim);
        }

        m_bActive = active;
    }

    void PrepareToSummonElemental()
    {
        DoCast(m_pElemental, SPELL_EMERGE, false);
        SetActive(false);
    }

    void DoAction(uint32)
    {
        SetActive(true);
    }

    void AttackStart(Unit* who)
    {
        if (!m_bActive)
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!m_bActive)
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
            pInstance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (SpellFreezeAnim)
        {
            if (SpellFreezeAnim<=diff)
            {
                m_creature->CastSpell(m_creature, SPELL_FREEZE_ANIM, true);
                SpellFreezeAnim=0;
            }
            else SpellFreezeAnim-=diff;

            return;
        }

        
        //Return since we have no target
        if (!m_bActive || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Summoned == false && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() <= 50 && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 6)
        {
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            x=x+cos(m_creature->GetOrientation())*9;
            y=y+sin(m_creature->GetOrientation())*9;
            m_pElemental = m_creature->SummonCreature(NPC_DRAKKARI_ELEMENTAL, x, y, z, m_creature->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
            
            PrepareToSummonElemental();

            Summoned = true;
            return;
        }

        if (Summoned == true && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() <= 5)
        {
            if (!m_pElemental)
            {
                Summoned=false;
                return;
            }

            m_creature->SetInFront(m_pElemental);
            m_pElemental->SetInFront(m_creature);

            PrepareToSummonElemental();

            Summoned = false;

            Unit* pVictim = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);

            if (!pVictim)
                return;

            /* May be move to this NPC possition? */
            m_pElemental->SetVisibility(VISIBILITY_ON);
            m_pElemental->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_pElemental->GetMotionMaster()->MoveChase(pVictim);
            m_pElemental->CastSpell(m_pElemental,65208,true);
            m_pElemental->AI()->AttackStart(pVictim);
            return;
        }

        if (MightyBlowTimer <= diff)
        {
            if (!m_creature->hasUnitState(UNIT_STAT_STUNNED))
                DoCast(m_creature->getVictim(), SPELL_MIGHTY_BLOW, true);

            MightyBlowTimer = 10000;
        } else MightyBlowTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (pInstance)
            pInstance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, DONE);
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry()==NPC_DRAKKARI_ELEMENTAL)
        {
            summoned->CastSpell(summoned,65208,true);
            summoned->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
        }
    }
};

struct MANGOS_DLL_DECL boss_drakkari_elementalAI : public ScriptedAI
{
    boss_drakkari_elementalAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 SurgeTimer;
    uint16 PuddleTimer;
    uint16 DelayedVisibility;
    bool m_bMergedOnce;

    void EnterEvadeMode()
    {
        if (m_pInstance)
        {
            if (Creature* colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
                colossus->AI()->EnterEvadeMode();
        }
    }

    void Reset()
    {
        SurgeTimer = 7000;
        PuddleTimer=5000;
        m_bMergedOnce = false;
        DelayedVisibility=0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (DelayedVisibility)
        {
            if (DelayedVisibility<=diff)
            {
                m_creature->SetVisibility(VISIBILITY_OFF);
                if (Creature* colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
                    colossus->AI()->DoAction(1);
                DelayedVisibility=0;
            }
            else DelayedVisibility-=diff;

            return;
        }
        
        //Return since we have no target
        if(m_creature->GetVisibility() == VISIBILITY_OFF || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->HasAura(SPELL_MOJO_VOLLEY))
        {
            DoCast(m_creature,SPELL_MOJO_VOLLEY);
        }

        if(!m_bMergedOnce && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() <= 50)
        {
            m_bMergedOnce = true;

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveAllAuras();
            DelayedVisibility=3000;
            m_creature->GetMotionMaster()->MoveIdle();
            if (m_pInstance)
            {
                if (Creature* colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
                    m_creature->CastSpell(colossus, SPELL_MERGE, false);
            }
            m_creature->RemoveAurasDueToSpell(SPELL_MOJO_VOLLEY);
        }

        /*if (PuddleTimer<diff)
        {
            ThreatList const& tList = m_creature->getThreatManager().getThreatList();

            for (ThreatList::const_iterator i = tList.begin();i != tList.end(); ++i)
            {
                Unit* pTarget = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());

                if (pTarget)
                    m_creature->CastSpell(pTarget,58994,true);
            }
            PuddleTimer=5000;
        }
        else PuddleTimer-=diff;*/

        /*if (SurgeTimer <= diff) 
        {
            DisgustingSurgeHack();

            SurgeTimer = 7000;
        } else SurgeTimer -= diff;*/  //the spell isn't working well, removed

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (Creature* Colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
        {
            ((boss_drakkari_colossusAI*)m_creature->AI())->SetActive(true);
            Colossus->DealDamage(Colossus, Colossus->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
};

struct MANGOS_DLL_DECL npc_living_mojoAI : public ScriptedAI
{
    npc_living_mojoAI(Creature *c) : ScriptedAI(c)
    {
        HeroicMode = c->GetMap()->GetDifficulty();
        m_bActive  = !c->IsInRange3d(1672.959f, 743.487f, 143.337f, 0.0f, 17.0f);
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    bool HeroicMode;
    bool m_bActive;

    uint32 MojoWaveTimer;
    uint32 MojoPuddleTimer;

    ScriptedInstance* m_pInstance;

    void MoveInLineOfSight(Unit* who)
    {
        if (!m_bActive)
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (!m_bActive)
            return;

        ScriptedAI::AttackStart(who);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if (!m_bActive && uiMoveType == POINT_MOTION_TYPE && uiPointId == POINT_DRAKKARI_COLOSSUS && m_pInstance)
        {
            if (Creature* colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
                ((boss_drakkari_colossusAI*)colossus->AI())->AddMojo();
            m_creature->SetVisibility(VISIBILITY_OFF);
        }
    }

    void Reset()
    {
        MojoWaveTimer = 2000;
        MojoPuddleTimer = 7000;

        m_creature->SetVisibility(VISIBILITY_ON);

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DamageTaken(Unit* pDone_by, uint32& uiDamage)
    {
        if (m_bActive || !m_pInstance)
            return;

        if (Creature* colossus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_DRAKKARI_COLOSSUS)))
            ((boss_drakkari_colossusAI*)colossus->AI())->StartEvent();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_bActive || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (MojoWaveTimer <= diff)
        {
            DoCast(m_creature->getVictim(), HEROIC(SPELL_MOJO_WAVE, H_SPELL_MOJO_WAVE));
            MojoWaveTimer = 15000;
        } else MojoWaveTimer -= diff;

        if (MojoPuddleTimer <= diff)
        {
            DoCast(m_creature->getVictim(), HEROIC(SPELL_MOJO_PUDDLE, H_SPELL_MOJO_PUDDLE));
            MojoPuddleTimer = 18000;
        } else MojoPuddleTimer -= diff;

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_drakkari_colossus(Creature* pCreature)
{
    return new boss_drakkari_colossusAI (pCreature);
}

CreatureAI* GetAI_boss_drakkari_elemental(Creature* pCreature)
{
    return new boss_drakkari_elementalAI (pCreature);
}

CreatureAI* GetAI_npc_living_mojo(Creature* pCreature)
{
    return new npc_living_mojoAI (pCreature);
}

void AddSC_boss_drakkari_colossus()
{
    Script *newscript;

    newscript = new Script();
    newscript->Name = "boss_drakkari_colossus";
    newscript->GetAI = &GetAI_boss_drakkari_colossus;
    newscript->RegisterSelf();

    newscript = new Script();
    newscript->Name = "boss_drakkari_elemental";
    newscript->GetAI = &GetAI_boss_drakkari_elemental;
    newscript->RegisterSelf();

    newscript = new Script();
    newscript->Name = "npc_living_mojo";
    newscript->GetAI = &GetAI_npc_living_mojo;
    newscript->RegisterSelf();
}
