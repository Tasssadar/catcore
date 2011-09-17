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
    
    SPELL_TELEPORT_VISUAL = 64446,
    SPELL_CAMERA_SHAKE    = 69235,
    
    NPC_ADD             = 45000,
    NPC_GUIDE           = 60000,
    NPC_STONE_GIANT     = 60001,
    NPC_COREN           = 23872, 
    
    PHASE_NOSTART       = 0,
    PHASE_TAUNT         = 1,
    PHASE_WAITING       = 2, 
    PHASE_FIGHT_FIRST   = 3,
    
};

const static float firstMobPos[][4] = 
{
    { 774.48, -166.12, -32.9, 2.4}, // left
    { 768.62, -163.00, -32.9, 2.4}, // right
};

const static float firstMobJumpPos[][3] = 
{
    { 759.9, -152.5, -73 }, // left
    { 771.49, -138.8, -73 }, // right
};

const static float bossPositions[][4] = 
{
    { 878.16, -249.23, -49.9, 4.45},
};

struct MANGOS_DLL_DECL boss_coren_direbrewAI : public ScriptedAI
{
    boss_coren_direbrewAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPhase = 0;
        m_creature->SetRespawnTime(86400);
        Reset();
    }

    uint32 m_uiPullTimer;
    uint32 m_uiSmashTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiAddCheck_Timer;
    uint32 m_uiBlizzard_Timer;
    uint32 m_uiRange_Timer;
    
    uint8 m_uiPhase;
    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    
    Creature *pFirstAdds[2];

    void Reset()
    {
        m_uiPullTimer = 15000;
        m_uiSmashTimer = 17000;
        m_uiCleaveTimer = urand(5000, 8000);
        
        m_uiAddCheck_Timer = 2000;
        m_uiBlizzard_Timer = 10000;
        
        m_uiRange_Timer = 10000;        
        m_uiEventTimer = 2000;
        m_uiEventPhase = PHASE_TAUNT;
        pFirstAdds[0] = NULL;
        pFirstAdds[1] = NULL;
        SetCombatMovement(false);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }
    
    void AttackStart(Unit* pWho)
    {
        if(m_uiPhase != PHASE_FIGHT_FIRST)
            return;

        ScriptedAI::AttackStart(pWho);
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

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case PHASE_NOSTART: return;
            case PHASE_WAITING: return;
            case PHASE_TAUNT:
            {
                if(m_uiEventTimer <= uiDiff)
                {
                    switch(m_uiEventPhase)
                    {
                        case 0:
                            //TODO: text
                            m_uiEventTimer = 2000;
                            break;
                        case 1:
                            //TODO: text
                            m_uiEventTimer = 4000;
                            break;
                        case 2:
                            //TODO: text 
                            m_uiEventTimer = 3000;
                            break;
                        case 3:
                        {
                            for(uint8 i = 0; i < 2; ++i)
                            {
                                pFirstAdds[i] = m_creature->SummonCreature(NPC_STONE_GIANT, firstMobPos[i][0], firstMobPos[i][1], firstMobPos[i][2], firstMobPos[i][3],
                                                                           TEMPSUMMON_CORPSE_DESPAWN, 5000);
                            }
                            m_uiEventTimer = 1000;
                            break;
                        }
                        case 4:
                            // TODO:Text
                            for(uint8 i = 0; i < 2; ++i)
                            {
                                pFirstAdds[i]->TrajMonsterMove(firstMobJumpPos[i][0], firstMobJumpPos[i][1], firstMobJumpPos[i][2],
                                                               false, 50, 1500);
                            }
                            m_creature->CastSpell(m_creature, SPELL_TELEPORT_VISUAL, false);
                            m_uiEventTimer = 500;
                            break;
                        case 5:
                            m_creature->NearTeleportTo(bossPositions[0][0], bossPositions[0][1],bossPositions[0][2], bossPositions[0][3]);    
                            m_uiEventTimer = 1000;
                            m_uiPhase = PHASE_WAITING;
                            break;
                    }
                    ++m_uiEventPhase;
                }else m_uiEventTimer -= uiDiff;
            }
            default: return;
        }
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        DoMeleeAttackIfReady();
    }
};

static const float gatePos[3] = { 717.3, -100, -71.5 };
static const float goPos[3] = { 740.62, -122.2, -72.0 };
static const float bossPos[3] = { 779.9, -160.11, -32.9, 2.4 };

struct MANGOS_DLL_DECL npc_brewfest_guideAI : public ScriptedAI
{
    npc_brewfest_guideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPhase = 0;
        Reset();
    }
    
    uint8 m_uiPhase;
    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    bool stopped;

    void Reset()
    {
        stopped = true;
        m_uiEventTimer = 1000;
        m_uiEventPhase = 0;
    }
    
    void DoAction(uint32 action)
    {
        if(action == 0)
            stopped = false;
    }
    
    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if(uiMoveType != POINT_MOTION_TYPE)
            return;
        
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        
        switch(uiPointId)
        {
            case 1:
            case 2:
                stopped = false;
                break;
            case 3:
                m_creature->SetVisibility(VISIBILITY_OFF);
                break;
        }
    }
    
    void CastShake()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (plr && plr->IsInWorld())
                    plr->CastSpell(plr, SPELL_CAMERA_SHAKE, true);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(stopped)
            return;
        
        if(m_uiEventTimer <= uiDiff)
        {
            switch(m_uiEventPhase)
            {
                case 0:
                    //TODO: text
                    m_creature->MovePoint(1, gatePos[0], gatePos[1], gatePos[2]);
                    stopped = true;
                    break;
                case 1:
                {
                    // TODO:text
                    float x,y,z;
                    x += cos(m_creature->GetOrientation()+M_PI_F)*10;
                    y += sin(m_creature->GetOrientation()+M_PI_F)*10;
                    m_creature->UpdateGroundPositionZ(x, y, z, 50);
                    PointPath path;
                    path.resize(2);
                    path.set(0, m_creature->GetPosition());
                    path.set(1, Coords(x, y, z));
                    m_creature->ChargeMonsterMove(path, SPLINETYPE_NORMAL, SPLINEFLAG_BACKWARD, 2000);
                    m_uiEventTimer = 3000;
                    CastShake();
                    break;
                }
                case 2:
                case 3:
                    CastShake();
                    m_uiEventTimer = 3000;
                    break;
                case 4:
                    // TODO:text
                    stopped = true;
                    m_creature->MovePoint(2, goPos[0], goPos[1], goPos[2]);
                    break;
                case 5:
                    m_creature->SummonCreature(NPC_COREN, bossPos[0], bossPos[1], bossPos[2], bossPos[3],
                                               TEMPSUMMON_MANUAL_DESPAWN);
                    m_uiEventTimer = 10000;
                    break;
                case 6:
                    // TODO: text
                    m_creature->MovePoint(3, gatePos[0], gatePos[1], gatePos[2]);
                    break;
                
            }
            ++m_uiEventPhase;
        }else m_uiEventTimer -= uiDiff;
            
    }
};

bool GossipHello_npc_brewfest_guide(Player* pPlayer, Creature* pCreature)
{   
    pPlayer->ADD_GOSSIP_ITEM(0, "We are ready", GOSSIP_SENDER_MAIN, 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_brewfest_guide(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    if (uiAction == 1)
    {
        pCreature->AI()->DoAction(0);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return true;
}

CreatureAI* GetAI_boss_coren_direbrew(Creature* pCreature)
{
    return new boss_ahuneAI(pCreature);
}

CreatureAI* GetAI_npc_brewfest_guide(Creature* pCreature)
{
    return new npc_brewfest_guideAI(pCreature);
}

void AddSC_boss_coren_direbrew()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_coren_direbrew";
    newscript->GetAI = &GetAI_boss_ahune;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_brewfest_guide";
    newscript->GetAI = &GetAI_npc_brewfest_guide;
    NewScript->pGossipHello = &GossipHello_npc_brewfest_guide;
    NewScript->pGossipSelect = &GossipSelect_npc_brewfest_guide;
    newscript->RegisterSelf();
    
}