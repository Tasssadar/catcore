/* ScriptData
SDName: Frost Lord Ahune
SD%Complete: 100
SDComment: Custom boss
SDCategory: Bosses
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO           = -1000701,
    SAY_PULL            = -1000702,
    SAY_KILL1           = -1000703,
    SAY_KILL2           = -1000704,
    SAY_DEATH           = -1000705,
    
    SPELL_GRIP          = 70564,
    SPELL_SMASH         = 42669,
    SPELL_CLEAVE        = 19983,
    SPELL_ICE_BLOCK     = 69924,
    SPELL_LINK          = 54393,
    SPELL_DMG           = 36300,
    SPELL_BLIZZARD      = 28547,
    SPELL_FROST_BREATH  = 29318,
    
    SPELL_FROST_NOVA    = 33395,
    SPELL_SUMMON_AURA   = 43897,
    
    NPC_ADD             = 45000
    
};

struct MANGOS_DLL_DECL boss_ahuneAI : public ScriptedAI
{
    boss_ahuneAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiPullTimer;
    uint32 m_uiSmashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiAddCheck_Timer;
    uint32 m_uiBlizzard_Timer;
    uint32 m_uiRange_Timer;
    
    uint8 m_uiPhase;

    void Reset()
    {
        m_uiPullTimer = 15000;
        m_uiSmashTimer = 17000;
        m_uiCleaveTimer = urand(5000, 8000);
        
        m_uiAddCheck_Timer = 2000;
        m_uiBlizzard_Timer = 10000;
        
        m_uiRange_Timer = 10000;        
        m_uiPhase = 1;
        SetCombatMovement(false);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }
    
    void MoveInLineOfSight(Unit *pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || m_creature->GetDistance2d(pWho) > 50.0f || 
            !pWho->isTargetableForAttack())
            return;
        
        m_creature->AddThreat(pWho, 0.0f);
        m_creature->SetInCombatWith(pWho);
        
        PullAll();
    }
    
    void EnterEvadeMode()
    {
        CreatureList adds;
        GetCreatureListWithEntryInGrid(adds, m_creature, NPC_ADD, 50.0f);
        if (!adds.empty())
            for(CreatureList::iterator iter = adds.begin(); iter != adds.end(); ++iter)
                (*iter)->ForcedDespawn();

        m_creature->GetMotionMaster()->MoveTargetedHome();
    }
    
    void PullAll()
    {
        float fx, fy, fz;
        m_creature->GetPosition(fx, fy, fz);
        fx += cos(m_creature->GetOrientation())*5;
        fy += sin(m_creature->GetOrientation())*5;
        m_creature->UpdateGroundPositionZ(fx, fy, fz, 20.0f);
        
        Map* pMap = m_creature->GetMap();
        if (!pMap)
            return;
        Player *plr = NULL;
        WorldPacket data(SMSG_MONSTER_MOVE);
        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (!itr->getSource()->isAlive() || !itr->getSource()->IsInWorld())
                continue;
            plr = itr->getSource();
            
            float distance = plr->GetDistance(fx, fy, fz);
            uint32 traveltime = uint32(distance/(84.0f*0.001f));
            float velocity = (float((0.05f)*8)/float(pow(traveltime/1000.0f, 2.0f)))*10.0f;
            
            data.Initialize(SMSG_MONSTER_MOVE);
            data << plr->GetPackGUID();
            data << uint8(0);
            data << plr->GetPositionX() << plr->GetPositionY() << plr->GetPositionZ();
            data << uint32(getMSTime());
            data << uint8(SPLINETYPE_NORMAL);
            data << uint32(SPLINEFLAG_TRAJECTORY | SPLINEFLAG_WALKMODE | SPLINEFLAG_KNOCKBACK);
            data << uint32(traveltime);
            data << float(velocity);
            data << uint32(0);
            data << uint32(1);
            data << fx << fy << fz;
            plr->SendMessageToSet(&data, true);
            
            m_creature->CastSpell(plr, SPELL_GRIP, true);
        }
    }
    
    void SpawnAdds()
    {
        float fx, fy, fz;
        m_creature->GetPosition(fx, fy, fz);
        float angle = m_creature->GetOrientation();
        for(uint8 i = 0; i < 4; ++i)
        {
            float sx, sy, sz;
            sx = fx+cos(angle)*40.0f;
            sy = fy+sin(angle)*40.0f;
            sz = fz;
            m_creature->UpdateGroundPositionZ(sx, sy, sz, 20.0f);
            Creature *pAdd = m_creature->SummonCreature(NPC_ADD, sx, sy, sz, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            if (!pAdd)
                continue;
            pAdd->GetMotionMaster()->MovePoint(0, fx, fy, fz, false);
            pAdd->CastSpell(pAdd, SPELL_SUMMON_AURA, true);
            
            angle += M_PI_F/2;
            angle = (angle <= 2*M_PI_F) ? angle : angle - 2 * M_PI_F;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == 1 || m_uiPhase == 2)
        {
            // tank out of range
            if (m_creature->GetDistance2d(m_creature->getVictim()) > m_creature->GetAttackDistance(m_creature->getVictim()))
            {
                if (m_uiRange_Timer <= uiDiff)
                {
                    DoCast(m_creature, SPELL_FROST_BREATH);
                    m_uiRange_Timer = 10000;
                }m_uiRange_Timer -= uiDiff;
            }else m_uiRange_Timer = 10000;
            
            //Pull
            if (m_uiPullTimer < uiDiff)
            {
                PullAll();
                m_uiPullTimer = 15000;
            }
            else m_uiPullTimer -= uiDiff;

            //Smash
            if (m_uiSmashTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_SMASH);
                m_uiSmashTimer = 17000;
            }
            else m_uiSmashTimer -= uiDiff;

            //Cleave
            if (m_uiCleaveTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
                m_uiCleaveTimer = urand(5000, 8000);
            }
            else m_uiCleaveTimer -= uiDiff;
            
            if (m_uiPhase == 1 && m_creature->GetHealthPercent() <= 50.0f)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveAllAuras();
                m_creature->CastSpell(m_creature, SPELL_ICE_BLOCK, true);
                SpawnAdds();
                m_uiPhase = 2;
                return;
            }
        }
        else if (m_uiPhase == 2)
        {
            if (m_uiAddCheck_Timer <= uiDiff)
            {
                CreatureList adds;
                GetCreatureListWithEntryInGrid(adds, m_creature, NPC_ADD, 5.0f);
                if (!adds.empty())
                {
                    adds.clear();
                    GetCreatureListWithEntryInGrid(adds, m_creature, NPC_ADD, 45.0f);
                    for(CreatureList::iterator iter = adds.begin(); iter != adds.end(); ++iter)
                    {
                        if (!(*iter)->isAlive())
                            continue;
                        (*iter)->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
                        m_creature->CastSpell(*iter, SPELL_LINK, true);
                        m_creature->CastSpell(m_creature, SPELL_DMG, true);
                    }
                    m_uiPhase = 3;
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->RemoveAllAuras();
                    PullAll();
                    m_uiPullTimer = 15000;
                    m_uiSmashTimer = 17000;
                    m_uiCleaveTimer = urand(5000, 8000);
                    m_uiRange_Timer = 10000;
                    return;
                }
                m_uiAddCheck_Timer = 2000;
            }else m_uiAddCheck_Timer -= uiDiff;
            
            if (m_uiBlizzard_Timer <= uiDiff)
            {
                if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target,SPELL_BLIZZARD);
                m_uiBlizzard_Timer = 10000;
            }else m_uiBlizzard_Timer -= uiDiff;
            
            //we dont wanna melee
            return;
        }
        
        DoMeleeAttackIfReady();
    }
};


struct MANGOS_DLL_DECL mob_ahune_addAI : public ScriptedAI
{
    mob_ahune_addAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiNovaTimer;
    bool m_bStart;

    void Reset()
    {
        m_uiNovaTimer = urand(10000, 25000);
        
        m_bStart = false;
    }
    
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LINK)
        {
            m_creature->SetMaxHealth(200000);
            m_creature->SetHealth(200000);
            m_creature->RemoveAllAuras();
            m_bStart = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_bStart || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (m_uiNovaTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_FROST_NOVA);
            m_uiNovaTimer = urand(10000, 25000);
        }else m_uiNovaTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ahune(Creature* pCreature)
{
    return new boss_ahuneAI(pCreature);
}

CreatureAI* GetAI_mob_ahune_add(Creature* pCreature)
{
    return new mob_ahune_addAI(pCreature);
}

void AddSC_boss_ahune()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_ahune";
    newscript->GetAI = &GetAI_boss_ahune;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "mob_ahune_add";
    newscript->GetAI = &GetAI_mob_ahune_add;
    newscript->RegisterSelf();
}