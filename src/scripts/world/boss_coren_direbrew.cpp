/* ScriptData
SDName: Frost Lord Ahune
SD%Complete: 100
SDComment: Custom boss
SDCategory: Bosses
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_GUIDE1          = -1230999,
    SAY_GUIDE2          = -1230998,
    SAY_GUIDE3          = -1230997,
    SAY_GUIDE4          = -1230996,
    
    SAY_INTRO1          = -1230995,
    SAY_INTRO2          = -1230994,
    SAY_INTRO3          = -1230993,
    SAY_INTRO4          = -1230992,
    
    SAY_AGGRO           = -1230991,
    SAY_START_CAMERA    = -1230990,
    SAY_TAVERN1         = -1230989,
    SAY_TAVERN2         = -1230988,
    
    SAY_URSULA0         = -1230982,
    SAY_URSULA1         = -1230987,
    SAY_URSULA2         = -1230986,
    SAY_URSULA3         = -1230985,
    SAY_URSULA4         = -1230984,
    SAY_URSULA5         = -1230983,
    SAY_URSULA6         = -1230981,
    
    SAY_PULL            = -1000702,
    SAY_KILL1           = -1000703,
    SAY_KILL2           = -1000704,
    SAY_DEATH           = -1230980,
    
    SPELL_SMASH         = 59706, 
    SPELL_BLAST_WAVE    = 71151,
    SPELL_GROW          = 74998,
    SPELL_STUN          = 23775, 
    SPELL_BLIZZARD      = 62576,
    
    SPELL_TELEPORT_VISUAL = 64446,
    SPELL_CAMERA_SHAKE    = 69235,
    
    NPC_GUIDE           = 60000,
    NPC_STONE_GIANT     = 60001,
    NPC_COREN           = 23872, 
    NPC_CAMERA          = 60002,
    NPC_BARREL          = 60003,
    NPC_URSULA          = 26822,
    
    GO_DOORS            = 157923, 
    
    PHASE_NOSTART       = 0,
    PHASE_TAUNT         = 1,
    PHASE_WAITING       = 2, 
    PHASE_FIGHT_FIRST   = 3,
    PHASE_MOVING        = 4,
    PHASE_MOVIE         = 5,
    PHASE_URSULA        = 6,
    PHASE_FIGHT_SECOND  = 7,
    PHASE_FIGHT_THIRD   = 8,
};

const static float firstMobPos[][4] = 
{
    { 774.48, -166.12, -32.9, 2.4}, // left
    { 785.62, -152.95, -32.9, 2.4}, // right
};

const static float firstMobJumpPos[][3] = 
{
    { 759.9, -152.5, -73 }, // left
    { 771.49, -138.8, -73 }, // right
};

const static float bossPositions[][4] = 
{
    { 771.49, -150.49, -73, 0},
    { 891.91, -270.68, -71.8, 0},
    { 915.94, -252.35, -48.9, 3.94},
    { 853.07, -162.83, -49.75, 0},
    { 896.10, -143.21, -49.75, 3.66},
};

#define MAX_SPAWNS 4

static const uint32 addIds[3] = { 65000, 65001, 65002 };
static const float addSpawnPos[][3] = 
{
    {841.89, -218.71, -72.04},
    {949.32, -328.02, -71.75}
};

struct MANGOS_DLL_DECL boss_coren_direbrewAI : public ScriptedAI
{
    boss_coren_direbrewAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPhase = PHASE_TAUNT;
        m_creature->SetRespawnTime(86400);
        Reset();
    }

    uint32 m_uiSmashTimer;
    uint32 m_uiBarrelTimer;
    
    uint32 m_uiSpawnTimer;
    uint8 m_uiSpawnCount;
    
    uint8 m_uiPhase;
    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    
    Creature *pFirstAdds[2];
    //std::vector<Creature*> CameraList;
    Creature *pUrsula;

    void Reset()
    {
        m_uiSmashTimer = 10000;
        m_uiBarrelTimer = 15000;
        
        m_uiSpawnTimer = 2000;     
        m_uiSpawnCount = 0;
        
        m_uiEventTimer = 2000;
        m_uiEventPhase = 0;
        pFirstAdds[0] = NULL;
        pFirstAdds[1] = NULL;

        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        SetCombatMovement(false);
        //CameraList.clear();
        pUrsula = NULL;
    }

    void Aggro(Unit*)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }
    
    void JustDied(Unit*)
    {
        if(pUrsula)
            pUrsula->ForcedDespawn();
        DoScriptText(SAY_DEATH, m_creature);
    }
    
    void AttackStart(Unit* pWho)
    {
        //if(m_uiPhase < PHASE_FIGHT_SECOND)
          //  return;

        ScriptedAI::AttackStart(pWho);
    }
    
    void EnterEvadeMode()
    {
        if(pUrsula)
            pUrsula->ForcedDespawn();
    }
    
    void MoveInLineOfSight(Unit *pWho)
    {
        if(m_uiPhase != PHASE_WAITING || !pWho || pWho->GetTypeId() != TYPEID_PLAYER || m_creature->GetDistance2d(pWho) > 50.0f
            || !m_creature->IsWithinLOSInMap(pWho))
            return;
        
        ScriptedAI::AttackStart(pWho);
        m_uiPhase = PHASE_FIGHT_FIRST;          
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
                m_uiPhase = PHASE_WAITING;
                m_creature->NearTeleportTo(bossPositions[2][0], bossPositions[2][1],bossPositions[2][2], bossPositions[2][3]);
                break;
            case 2:
               // m_uiEventTimer = 0;
                break;
        }
    }
    
    void DoSpawnGroup()
    {
        //TODO
        if(m_uiSpawnCount >= MAX_SPAWNS)
        {
            m_uiPhase = PHASE_MOVIE;
            SpawnCameras();
            m_creature->GetMotionMaster()->MovePoint(2, bossPositions[3][0], bossPositions[3][1],bossPositions[3][2]);    
            m_uiEventTimer = 17000;
            m_uiEventPhase = 0;
            DoScriptText(SAY_START_CAMERA, m_creature);
            return;
        }
        for(uint8 i = 0; i < 2; ++i)
        {
            uint8 add = urand(0, 2);
            Creature *pAdd = m_creature->SummonCreature(addIds[add], addSpawnPos[0][0], addSpawnPos[0][1], addSpawnPos[0][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pAdd->Attack(m_creature->getVictim(), true))
            {
                pAdd->AddThreat(m_creature->getVictim());
                pAdd->SetInCombatWith(m_creature->getVictim());
                m_creature->getVictim()->SetInCombatWith(pAdd);
                pAdd->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
        }
        
        for(uint8 i = 0; i < 2; ++i)
        {
            uint8 add = urand(0, 2);
            Creature *pAdd = m_creature->SummonCreature(addIds[add], addSpawnPos[1][0], addSpawnPos[1][1], addSpawnPos[1][2], 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pAdd->Attack(m_creature->getVictim(), true))
            {
                pAdd->AddThreat(m_creature->getVictim());
                pAdd->SetInCombatWith(m_creature->getVictim());
                m_creature->getVictim()->SetInCombatWith(pAdd);
                pAdd->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
        }
        
        ++m_uiSpawnCount;
        m_uiSpawnTimer = 35000;
    }
    
    void SpawnCameras()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (plr && plr->IsInWorld())
                {
                    float x, y, z;
                    plr->GetPosition(x, y, z);
                    Creature *camera = m_creature->SummonCreature(NPC_CAMERA, x, y, z, plr->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                    plr->GetCamera().SetView(camera);
                    //CameraList.push_back(camera);
                    plr->SetClientControl(plr, 0);
                    camera->AI()->DoAction(10);
                    m_creature->GetRandomPoint(bossPositions[4][0], bossPositions[4][1], bossPositions[4][2], 8, x, y, z);
                    plr->NearTeleportTo(x, y, z, bossPositions[4][3]);
                    plr->CombatStop();
                    if(!plr->isAlive())
                    {
                        plr->ResurrectPlayer(100.0f);
                        plr->SpawnCorpseBones();
                    }
                }
            }
        }
    }
    
    void KnockBackAll()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (plr && plr->IsInWorld())
                {
                    plr->KnockBackFrom(m_creature, 20.0f, 10.0f);
                    plr->CastSpell(plr, SPELL_STUN, true);
                }
            }
        }
    }
    
    void RemoveStun()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (plr && plr->IsInWorld())
                    plr->RemoveAurasDueToSpell(SPELL_STUN);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case PHASE_NOSTART: return;
            case PHASE_WAITING: return;
            case PHASE_MOVING: return;
            case PHASE_TAUNT:
            {
                if(m_uiEventTimer <= uiDiff)
                {
                    switch(m_uiEventPhase)
                    {
                        case 0:
                            DoScriptText(SAY_INTRO1, m_creature);
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_uiEventTimer = 2000;
                            break;
                        case 1:
                            DoScriptText(SAY_INTRO2, m_creature);
                            m_uiEventTimer = 4000;
                            break;
                        case 2:
                            DoScriptText(SAY_INTRO3, m_creature);
                            m_uiEventTimer = 5000;
                            break;
                        case 3:
                            m_uiEventTimer = 7000;
                            DoScriptText(SAY_INTRO4, m_creature);
                            for(uint8 i = 0; i < 2; ++i)
                            {
                                pFirstAdds[i] = m_creature->SummonCreature(NPC_STONE_GIANT, firstMobPos[i][0], firstMobPos[i][1], firstMobPos[i][2], firstMobPos[i][3],
                                                                           TEMPSUMMON_CORPSE_DESPAWN, 5000);
                            }
                            break;
                        case 4:
                            for(uint8 i = 0; i < 2; ++i)
                            {
                                pFirstAdds[i]->TrajMonsterMove(firstMobJumpPos[i][0], firstMobJumpPos[i][1], firstMobJumpPos[i][2],
                                                               false, 50, 1500);
                            }
                            m_creature->TrajMonsterMove(bossPositions[0][0], bossPositions[0][1],bossPositions[0][2],
                                                               false, 50, 1500);
                            m_uiEventTimer = 1500;
                            break;
                        case 5:
                            m_creature->GetMotionMaster()->MovePoint(1, bossPositions[1][0], bossPositions[1][1],bossPositions[1][2]);    
                            m_uiPhase = PHASE_MOVING;
                            break;
                    }
                    ++m_uiEventPhase;
                }else m_uiEventTimer -= uiDiff;
                break;
            }
            case PHASE_FIGHT_FIRST:
            {
                if(m_uiSpawnTimer <= uiDiff)
                {
                    DoSpawnGroup();
                }else m_uiSpawnTimer -= uiDiff;
                break;
            }
            case PHASE_MOVIE:
            {
                if(m_uiEventTimer <= uiDiff)
                {
                    switch(m_uiEventPhase)
                    {
                        case 0:
                        {
                            Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                            if(!lPlayers.isEmpty())
                            {
                                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                {
                                    Player *plr = itr->getSource();
                                    if (plr && plr->IsInWorld())
                                    {
                                        float x, y, z;
                                        plr->GetPosition(x, y, z);
                                        x += cos(plr->GetOrientation())*40;
                                        y += sin(plr->GetOrientation())*40;
                                        m_creature->UpdateGroundPositionZ(x, y, z, 50.0f);
                                        plr->GetMotionMaster()->MovePoint(0, x, y, z);
                                    }
                                }
                            }
                            m_uiEventTimer = 2000;
                            break;
                        }
                        case 1:
                            DoScriptText(SAY_TAVERN1, m_creature);
                            m_uiEventTimer = 2000;
                            break;
                        case 2:
                        {
                            Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                            if(!lPlayers.isEmpty())
                                DoScriptText(SAY_TAVERN2, lPlayers.begin()->getSource());
                            m_uiEventTimer = 5000;
                            break;
                        }
                        case 3:
                            SetCombatMovement(true);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_uiPhase = PHASE_FIGHT_SECOND;
                            AttackStart(m_creature->getVictim());
                            break;
                    }
                    ++m_uiEventPhase;
                }else m_uiEventTimer -= uiDiff;
                break;
            }
            case PHASE_FIGHT_SECOND:
                if(m_creature->GetHealthPercent() <= 50.0f)
                {
                    KnockBackAll();
                    DoScriptText(SAY_URSULA0, m_creature);
                    m_uiPhase = PHASE_URSULA;
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->RemoveAllAuras();
                    m_uiEventPhase = 0;
                    m_uiEventTimer = 1000;
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveIdle();
                }
            case PHASE_FIGHT_THIRD:
            {
                //Return since we have no target
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;
                
                if(m_uiSmashTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SMASH);
                    m_uiSmashTimer = urand(10000, 17000);
                }else m_uiSmashTimer -= uiDiff;
                
                if(m_uiBarrelTimer <= uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        m_creature->SummonCreature(NPC_BARREL, x, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                    }
                    m_uiBarrelTimer = urand(10000, 17000);
                }else m_uiBarrelTimer -= uiDiff;
                
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_URSULA:
            {
                if(m_uiEventTimer <= uiDiff)
                {
                    switch(m_uiEventPhase)
                    {
                        case 0:
                        {
                            float x, y, z;
                            m_creature->GetPosition(x, y, z);
                            x += cos(0)*5;
                            y += sin(0)*5;
                            pUrsula = m_creature->SummonCreature(NPC_URSULA, x, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                            m_creature->SetTargetGUID(0);
                            m_creature->SetOrientation(m_creature->GetAngle(pUrsula));
                            pUrsula->SetOrientation(pUrsula->GetAngle(m_creature));
                            pUrsula->SendHeartBeatMsg();
                            m_creature->SendHeartBeatMsg();
                            DoScriptText(SAY_URSULA1, pUrsula);
                            m_uiEventTimer = 4000;
                            break;
                        }
                        case 1:
                            DoScriptText(SAY_URSULA2, m_creature);
                            m_uiEventTimer = 8000;
                            break;
                        case 2:
                            DoScriptText(SAY_URSULA4, m_creature);
                            m_uiEventTimer = 5000;
                            break;
                        case 3:
                            DoScriptText(SAY_URSULA6, m_creature);
                            m_uiPhase = PHASE_FIGHT_THIRD;
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            SetCombatMovement(true);
                            m_creature->GetMotionMaster()->Clear(false, true);
                            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                            RemoveStun();
                            pUrsula->AI()->AttackStart(m_creature->getVictim());
                            break;
                    }
                    ++m_uiEventPhase;
                }else m_uiEventTimer -= uiDiff;
                break;
            }
            default: return;
        }
    }
};

static const float gatePos[3] = { 717.3, -100, -71.5 };
static const float goPos[3] = { 740.62, -122.2, -72.0 };
static const float bossPos[4] = { 779.9, -160.11, -32.9, 2.4 };

struct MANGOS_DLL_DECL npc_brewfest_guideAI : public ScriptedAI
{
    npc_brewfest_guideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiPhase = 0;
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
    
    ScriptedInstance* m_pInstance;
    
    uint8 m_uiPhase;
    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    bool stopped;
    
    Creature *pCoren;
    
    std::vector<Creature*> CameraList;

    void Reset()
    {
        stopped = true;
        m_uiEventTimer = 1000;
        m_uiEventPhase = 0;
        if(GameObject *doors = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_DOORS)))
        {
            doors->SetLootState(GO_READY);
            doors->SetGoState(GO_STATE_READY);
        }
        CameraList.clear();
        pCoren = NULL;
    }
    
    void DoAction(uint32 action)
    {
        if(action == 0)
        {
            if(pCoren)
                pCoren->ForcedDespawn();
            stopped = false;
        }
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
            {
                /*if(GameObject *doors = GetClosestGameObjectWithEntry(m_creature, GO_DOORS, 180.0f))
                {
                    doors->SetLootState(GO_READY);
                    doors->SetGoState(GO_STATE_READY);
                } */
                //m_creature->SetVisibility(VISIBILITY_OFF);
                break;
            }
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
    
    void SetCameras()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (!plr || !plr->IsInWorld())
                    continue;
                switch(m_uiEventPhase)
                {
                    case 0:
                        plr->GetCamera().SetView(m_creature);
                        plr->SetClientControl(plr, 0);
                        break;
                    case 7:
                    {
                        float x, y, z;
                        m_creature->GetPosition(x, y, z);
                        Creature *camera = m_creature->SummonCreature(NPC_CAMERA, x, y, z, plr->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                        plr->GetCamera().SetView(camera);
                        CameraList.push_back(camera);
                        plr->SetClientControl(plr, 0);
                        camera->AI()->DoAction(0);
                        break;
                    }
                }
            }
        }
    }
    
    void MovePlayers()
    {
        
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if(!lPlayers.isEmpty())
        {
            float angle_add;
            float angle;
            
            switch(m_uiEventPhase)
            {
                case 0:
                    angle_add = M_PI_F/lPlayers.getSize();
                    angle = m_creature->GetOrientation()+M_PI_F/2;
                    angle = angle > M_PI_F*2 ? angle - M_PI_F*2 : angle;
                    break;
                case 6:
                    angle = m_creature->GetOrientation()-M_PI_F/2;
                    angle_add = M_PI_F/lPlayers.getSize();
                    angle = angle < 0 ? angle + M_PI_F*2 : angle;
                    break;
            }
            
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                Player *plr = itr->getSource();
                if (!plr || !plr->IsInWorld())
                    continue;
                switch(m_uiEventPhase)
                {
                    
                    case 0:
                    {
                        float x = gatePos[0] + cos(angle)*4;
                        float y = gatePos[1] + sin(angle)*4;
                        plr->GetMotionMaster()->MovePoint(1, x, y, gatePos[2]);
                        angle += angle_add;
                        angle = angle > M_PI_F*2 ? angle - M_PI_F*2 : angle;
                        break;
                    }
                    case 2:
                        plr->KnockBackFrom(plr, -10.0f, 10.0f);
                        break;
                    case 6:
                    {
                        float x = goPos[0] + cos(angle)*4;
                        float y = goPos[1] + sin(angle)*4;
                        plr->GetMotionMaster()->MovePoint(1, x, y, goPos[2]);
                        angle += angle_add;
                        angle = angle > M_PI_F*2 ? angle - M_PI_F*2 : angle;
                        break;
                    }
                }
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
                    DoScriptText(SAY_GUIDE1, m_creature);
                    m_creature->GetMotionMaster()->MovePoint(1, gatePos[0], gatePos[1], gatePos[2]);
                    stopped = true;
                    SetCameras();
                    MovePlayers();
                    break;
                case 1:
                    m_creature->HandleEmote(28);
                    m_uiEventTimer = 1000;
                    break;
                case 2:
                {
                    m_creature->HandleEmote(0);
                    DoScriptText(SAY_GUIDE2, m_creature);
                    if(GameObject *doors = GetClosestGameObjectWithEntry(m_creature, GO_DOORS, 180.0f))
                    {
                        doors->SetLootState(GO_READY);
                        doors->SetGoState(GO_STATE_ACTIVE);
                    }
        
                    float x,y,z;
                    m_creature->GetPosition(x,y,z);
                    x += cos(m_creature->GetOrientation()+M_PI_F)*10;
                    y += sin(m_creature->GetOrientation()+M_PI_F)*10;
                    m_creature->UpdateGroundPositionZ(x, y, z, 50);
                    m_creature->TrajMonsterMove(x, y, z, true, 10, 1500);
                    m_uiEventTimer = 3000;
                    MovePlayers();
                    CastShake();
                    break;
                }
                case 3:
                case 4:
                case 5:
                    CastShake();
                    m_uiEventTimer = 3000;
                    break;
                case 6:
                    stopped = true;
                    DoScriptText(SAY_GUIDE3, m_creature);
                    m_creature->GetMotionMaster()->MovePoint(2, goPos[0], goPos[1], goPos[2]);
                    MovePlayers();
                    break;
                case 7:
                    pCoren = m_creature->SummonCreature(NPC_COREN, bossPos[0], bossPos[1], bossPos[2], bossPos[3],
                                               TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);
                    m_uiEventTimer = 14000;
                    SetCameras();
                    break;
                case 8:
                {
                    for(std::vector<Creature*>::iterator itr = CameraList.begin(); itr != CameraList.end(); ++itr)
                        (*itr)->AI()->DoAction(1);
                    m_uiEventTimer = 1000;
                    break;
                }
                case 9:
                {
                    DoScriptText(SAY_GUIDE4, m_creature);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->GetMotionMaster()->MovePoint(3, gatePos[0], gatePos[1], gatePos[2]);
                    m_uiEventTimer = 2000;
                    break;
                }
                case 10:
                {
                    for(std::vector<Creature*>::iterator itr = CameraList.begin(); itr != CameraList.end(); ++itr)
                        (*itr)->AI()->DoAction(2);
                    
                    m_uiEventTimer = 1500;
                    break;
                }
                case 11:
                {
                    for(std::vector<Creature*>::iterator itr = CameraList.begin(); itr != CameraList.end(); ++itr)
                        (*itr)->AI()->DoAction(3);
                    m_uiEventTimer = 1500;
                    break;
                }
                case 12:
                {
                    for(std::vector<Creature*>::iterator itr = CameraList.begin(); itr != CameraList.end(); ++itr)
                        (*itr)->AI()->DoAction(4);
                    stopped = true;
                    break;
                }
            }
            ++m_uiEventPhase;
        }else m_uiEventTimer -= uiDiff;
            
    }
};


struct MANGOS_DLL_DECL npc_brewfest_cameraAI : public ScriptedAI
{
    npc_brewfest_cameraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        /*cameraPath.resize(7);
        cameraPath.set(1, Coords(902.82, -257.81, -36.60));
        cameraPath.set(2, Coords(870.67, -228.04, -39.72));
        cameraPath.set(3, Coords(865.34, -219.07, -41.08));
        cameraPath.set(4, Coords(882.06, -170.89, -40.64));
        cameraPath.set(5, Coords(884.06, -152.68, -42.63));
        cameraPath.set(6, Coords(875.84, -154.66, -48.56)); */
        Reset();
    }
    
    PointPath cameraPath;
    uint32 m_uiMoveTimer;
    uint8 m_movePhase;
    bool set;
    void Reset()
    {
        m_uiMoveTimer = 500;
        m_movePhase = 0;
        set = false;
    }
    
    void DoAction(uint32 id)
    {
        switch(id)
        {
            case 0:
                set = true;
                cameraPath.resize(2);
                cameraPath.set(1, Coords(bossPos[0], bossPos[1], bossPos[2]));
                m_movePhase = 10;
                break;
            case 1:
                m_creature->NearTeleportTo(756.83, -111.56, -72.3, 3.39);
                break;
            case 2:
            {
                float ori = bossPos[3] + M_PI_F;
                ori = ori > M_PI_F*2 ? ori - M_PI_F*2 : ori;
                m_creature->NearTeleportTo(bossPos[0], bossPos[1], bossPos[2], ori);
                break;
            }
            case 3:
                m_creature->TrajMonsterMove(bossPositions[0][0], bossPositions[0][1],bossPositions[0][2],
                                                               false, 50, 1500);
                break;
            case 4:
            {
                Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                if(!lPlayers.isEmpty())
                {
                    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    {
                        Player *plr = itr->getSource();
                       if (plr && plr->IsInWorld())
                       {
                            plr->SetClientControl(plr, 1);
                            plr->GetCamera().SetView(plr);
                        }
                    }
                }
                m_creature->ForcedDespawn();
                break;
            }
            case 10:
                cameraPath.resize(6);
                cameraPath.set(1, Coords(902.82, -257.81, -36.60));
                cameraPath.set(2, Coords(855.12, -232.85, -53.00));
                cameraPath.set(3, Coords(769.81, -149.82, -66.73));
                cameraPath.set(4, Coords(824.85, -77.59,  -48.27));
                cameraPath.set(5, Coords(851.86, -145.83, -49.75));
                set = true;
                m_movePhase = 0;
                break;
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(!set)
            return;
        
        if(m_uiMoveTimer <= uiDiff)
        {
            switch(m_movePhase)
            {
                case 0:
                    cameraPath.set(0, m_creature->GetPosition());
                    m_creature->ChargeMonsterMove(cameraPath, SPLINETYPE_NORMAL, SPLINEFLAG_TRAJECTORY, 15000);
                    m_uiMoveTimer = 25000;
                    break;
                case 1:
                {
                    Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                    if(!lPlayers.isEmpty())
                    {
                        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        {
                            Player *plr = itr->getSource();
                            if (plr && plr->IsInWorld())
                            {
                                plr->SetClientControl(plr, 1);
                                plr->GetCamera().SetView(plr);
                             }
                        }
                    }
                    m_creature->ForcedDespawn();
                    set = false;
                    break;
                }
                case 10:
                    cameraPath.set(0, m_creature->GetPosition());
                    m_creature->ChargeMonsterMove(cameraPath, SPLINETYPE_NORMAL, SPLINEFLAG_TRAJECTORY, 2000);
                    m_uiMoveTimer = 2000;
                    set = false;
                    break;
            }
           ++m_movePhase;
       }else m_uiMoveTimer -=uiDiff;
    }
};

bool GossipHello_npc_brewfest_guide(Player* pPlayer, Creature* pCreature)
{   
    pPlayer->ADD_GOSSIP_ITEM(0, "We are ready", GOSSIP_SENDER_MAIN, 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_brewfest_guide(Player* pPlayer, Creature* pCreature, uint32, uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    if (uiAction == 1)
    {
        pCreature->AI()->DoAction(0);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return true;
}


struct MANGOS_DLL_DECL npc_brewfest_barrelAI : public ScriptedAI
{
    npc_brewfest_barrelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    
    uint32 m_uiTimer;
    
    void Reset()
    {
        DoCast(m_creature, SPELL_GROW);
        m_uiTimer = 4000;
    }
    
    void AttackStart(Unit*)
    {
        return;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
       if(m_uiTimer <= uiDiff)
       {
           DoCast(m_creature, SPELL_BLAST_WAVE);
           m_creature->ForcedDespawn(200);
           m_uiTimer = 50000;
       }else m_uiTimer -=uiDiff;
    }
};

struct MANGOS_DLL_DECL boss_ursula_direbrewAI : public ScriptedAI
{
    boss_ursula_direbrewAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    
    uint32 m_uiTalkTimer;
    uint8 m_uiTalkPhase;
    bool talking;
    
    uint32 blizzardTimer;
    
    void Reset()
    {
        blizzardTimer = 8000;
        m_uiTalkTimer = 8000;
        talking = true;
        m_uiTalkPhase = 0;
    }
    
    void AttackStart(Unit* pWho)
    {
        if(talking)
            return;
        
        ScriptedAI::AttackStart(pWho);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(talking)
        {
            if(m_uiTalkTimer <= uiDiff)
            {
                switch(m_uiTalkPhase)
                {
                    case 0:
                        DoScriptText(SAY_URSULA3, m_creature);
                        m_uiTalkTimer = 7000;
                        break;
                    case 1:
                        DoScriptText(SAY_URSULA5, m_creature);
                        m_uiTalkTimer = 4000;
                        break;
                    case 2:
                        talking = false;
                        break;
                }
                ++m_uiTalkPhase;
            }else m_uiTalkTimer -=uiDiff;
            return;
        }
        
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(blizzardTimer <= uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_BLIZZARD);
            blizzardTimer = urand(15000, 24000);
        }else blizzardTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_coren_direbrew(Creature* pCreature)
{
    return new boss_coren_direbrewAI(pCreature);
}

CreatureAI* GetAI_npc_brewfest_guide(Creature* pCreature)
{
    return new npc_brewfest_guideAI(pCreature);
}

CreatureAI* GetAI_npc_brewfest_camera(Creature* pCreature)
{
    return new npc_brewfest_cameraAI(pCreature);
}

CreatureAI* GetAI_npc_brewfest_barrel(Creature* pCreature)
{
    return new npc_brewfest_barrelAI(pCreature);
}

CreatureAI* GetAI_boss_ursula_direbrew(Creature* pCreature)
{
    return new boss_ursula_direbrewAI(pCreature);
}

void AddSC_boss_coren_direbrew()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_coren_direbrew";
    newscript->GetAI = &GetAI_boss_coren_direbrew;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_brewfest_guide";
    newscript->GetAI = &GetAI_npc_brewfest_guide;
    newscript->pGossipHello = &GossipHello_npc_brewfest_guide;
    newscript->pGossipSelect = &GossipSelect_npc_brewfest_guide;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_brewfest_camera";
    newscript->GetAI = &GetAI_npc_brewfest_camera;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_brewfest_barrel";
    newscript->GetAI = &GetAI_npc_brewfest_barrel;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_ursula_direbrew";
    newscript->GetAI = &GetAI_boss_ursula_direbrew;
    newscript->RegisterSelf(); 
}

/*

INSERT INTO `scriptdev2`.`script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
('-1230999', 'Ok, lets see what is behind that giant door...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230998', 'Auch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '0', NULL),
('-1230997', 'Come in...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230996', 'Um...I\'ll wait outside', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230995', 'Hey ya, up here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230994', 'Wha\', ya think dwarves can\'t climb?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230993', 'Well, they can\'t, but when it comes to beer...ya know it', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230992', 'Anyway, look what I found here! Label said they are extremly powerful, wha\' \'bout some test?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230991', 'Looks like they were not so powerful after all, but that does not mean you can just wall into the tavern!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230990', 'Nah, I am tired of this, it\'s no fun when ya don\'t die. Bye, I\'m going to get some beer', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230989', 'WHAT?! How did you get here?!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '15', NULL),
('-1230988', 'We know magic, dwarf.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230987', 'What is goin\' on here?!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '15', NULL),
('-1230986', 'Ursula...um...hey, honey...don\'t worry, nothing serious, just these morons trying to ruin my rest time.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230985', 'And they are still alive? You need my help even for THAT?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '1', NULL),
('-1230984', 'I almost got \'em when you arrived!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230983', 'Excuses, nothin\' but exuses...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230982', 'Enough! I am tired of this, if you --', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230981', 'But...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL),
('-1230980', 'So much for \'Alcohol\'s safe\'', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '1', NULL);


*/















