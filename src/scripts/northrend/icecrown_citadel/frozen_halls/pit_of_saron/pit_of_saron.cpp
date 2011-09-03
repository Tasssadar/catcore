
#include "precompiled.h"
#include "pit_of_saron.h"

const float mobPosLeft[][3] =
{
   {450.228, 229.039, 528.709}, // lavo 37496 (37584)
   {451.704, 225.592, 528.709}, // lavo 37496 (37584)
   {448.348, 222.751, 528.708}, // lavo 37496 (37584)
   {446.051, 224.950, 528.708}, // lavo 37497 (37587)
   {443.588, 220.182, 528.708}, // lavo 37497 (37587)
};

const float mobPosRight[][3] =
{
   {448.292, 204.437, 528.706}, // pravo 37496 (37584)
   {451.279, 201.190, 528.706}, // pravo 37496 (37584)
   {450.860, 197.152, 528.707}, // pravo 37496 (37584)
   {448.359, 193.731, 528.708}, // pravo 37496 (37584)
   {446.766, 187.732, 528.707}, // pravo 37496 (37584)
   {449.695, 183.464, 528.706}, // pravo 37496 (37584)
   {445.419, 179.930, 528.707}, // pravo 37496 (37584)
   {445.702, 206.950, 528.707}, // pravo 37497 (37587)
   {445.088, 202.032, 528.706}, // pravo 37497 (37587)
   {445.348, 197.622, 528.706}, // pravo 37497 (37587)
   {443.485, 191.246, 528.754}, // pravo 37497 (37587)
};

const uint32 heroIds [][2] =
{
    {37498, 37587},
    {37496, 37584},
    {37497, 37588}, 
};

const uint32 necrolytePos[2][3] =
{
    {496.83, 198.89, 530.5}, // right
    {497.31, 247.85, 530.5}, // left
};


#define INTRO_MOB_COUNT_LEFT 5
#define INTRO_MOB_COUNT_RIGHT 11

/*
INSERT INTO `scriptdev2`.`script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
('-1658901', 'Chm! Farther, not event fit to laber in the quarry. Relish these final moments, for soon you will be nothing more the mindless undead.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16748', '1', '0', '0', ''),
('-1658900', 'Intruders have entered the Master''s domain. Signal the alarms!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16747', '1', '0', '0', 'Tyrranus opening'),
('-1658902', 'Heroes of the Alliance, attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16626', '1', '0', '0', ''),
('-1658903', 'Soldiers of the Horde, attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '17045', '1', '0', '0', ''),
('-1658904', 'Your last waking memory will be of agonazing pain!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16749', '1', '0', '0', ''),
('-1658905', 'Minions! Destroy these interlopers!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16751', '1', '0', '0', ''),
('-1658906', 'No, you monster!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16627', '1', '0', '0', ''),
('-1658907', 'Pathetic weaklings...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '17046', '1', '0', '0', ''),
('-1658908', 'You will have to make your way across this quarry on your own.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16629', '1', '0', '0', ''),
('-1658909', 'You will have to battle your way threw this pit on your own.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '17047', '1', '0', '0', ''),
('-1658910', 'Free any Alliance slaves that you come across. We will most certainly need their assistence in battling Tyrannus. I will gather reinforcemens and join you on the other side of the quarry.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '16630', '1', '0', '0', ''),
('-1658911', 'Free any Horde slaves that you come across. We will most certainly need their assistence in battling Tyrannus. I will gather reinforcemens and join you on the other side of the quarry.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '17048', '1', '0', '0', ''),
('-1658500', 'Forgemaster Garfrost hurls a massive saronite boulder at you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '5', '0', '0', ''),
('-1658501', 'Ick is chasing you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '5', '0', '0', ''),
('-1658502', 'Ick begins to unleash a toxic poison cloud!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '3', '0', '0', ''),
('-1658503', 'Krick begins rapidly conjuring explosive mines!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '3', '0', '0', ''),
('-1658504', 'Scourgelord Tyrannus roars and swells with dark might!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '3', '0', '0', ''),
('-1658505', 'The frostwyrm Rimefang gazes at %t and readies an icy attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '3', '0', '0', '');
*/
enum
{
    SAY_TYRANNUS_INTRO1     = -1658900,
    SAY_TYRANNUS_INTRO2     = -1658901,
    SAY_TYRANNUS_INTRO3     = -1658904,
    SAY_TYRANNUS_INTRO4     = -1658905,

    SOUND_TYRRANNUS_TRANSFORM = 16750,

    SPELL_STRANGULATE       = 69413,
    SPELL_TURN_TO_UNDEAD    = 69350,
    SPELL_NECROMANTIC_POWER = 69753,
    SPELL_SYLVANAS_PWNT     = 59514,
    SPELL_JAINA_PWNT        = 70464,

    NPC_SKELETAL_SLAVE      = 36881,

    NPC_FROST_BOMB          = 37186,
    SPELL_FROST_BOMB        = 70521,

    SPELL_TELE_ALI          = 70525,
    SPELL_TELE_HORDE        = 70639,

    SAY_SLAVE_INTRO_H         = -1658303,
    SAY_SLAVE_INTRO_A         = -1658319,
};

const int32 say_guide[][2] = 
{
    {-1658902, -1658903},
    {-1658906, -1658907},
    {-1658908, -1658909},
    {-1658910, -1658911},
};

const float portalPos[3] = { 427.84, 212.98, 529.2};
const float guideGoPos[3] = { 441.39f, 213.32f, 528.71f };

struct MANGOS_DLL_DECL mob_pos_guide_startAI : public ScriptedAI
{
    mob_pos_guide_startAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiExplodeTimer;
    uint32 m_uiEventTimer;
    uint8 faction;
    uint8 eventState;
    uint8 eventStateInternal;
    bool stopped;

    std::vector<Creature*> leftHerosList;
    std::vector<Creature*> rightHerosList;

    Creature *pTyrannus;

    void Reset()
    {
        if(!m_pInstance)
        {
            m_creature->ForcedDespawn();
            return;
        }
        faction = m_pInstance->GetData(TYPE_FACTION);
        eventState = m_pInstance->GetData(TYPE_EVENT_STATE);
        stopped = false;
        switch(eventState)
        {
            case 0:
                SpawnIntro();
                m_uiEventTimer = 5000;
                eventStateInternal = 0;
                pTyrannus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_TYRANNUS_INTRO));
                if(!pTyrannus)
                    m_creature->ForcedDespawn();
                break;
            default:
                stopped = true;
                break;
        }
    }

    void SetEventState(uint32 state)
    {
         eventState = state;
         m_pInstance->SetData(TYPE_EVENT_STATE, state);
    }

    void SpawnIntro()
    {
        Creature *tmp;
        for(uint8 i = 0; i < INTRO_MOB_COUNT_LEFT; ++i)
        {
            tmp = m_creature->SummonCreature(heroIds[urand(0, 2)][faction], portalPos[0], portalPos[1], portalPos[2],
                                       0.104f, TEMPSUMMON_DEAD_DESPAWN, 0);
            tmp->GetMotionMaster()->MovePoint(1, mobPosLeft[i][0], mobPosLeft[i][1], mobPosLeft[i][2]);
            
            leftHerosList.push_back(tmp);
        }
        for(uint8 i = 0; i < INTRO_MOB_COUNT_RIGHT; ++i)
        {
            tmp = m_creature->SummonCreature(heroIds[urand(0, 2)][faction], portalPos[0], portalPos[1], portalPos[2],
                                        0.104f, TEMPSUMMON_DEAD_DESPAWN, 0);
            tmp->GetMotionMaster()->MovePoint(1, mobPosRight[i][0], mobPosRight[i][1], mobPosRight[i][2]);
            rightHerosList.push_back(tmp);
        }
        tmp = m_creature->SummonCreature(NPC_NECROLYTE, necrolytePos[0][0], necrolytePos[0][1], necrolytePos[0][2],
                                         0.104f, TEMPSUMMON_DEAD_DESPAWN, 0);
        rightHerosList.push_back(tmp);
        tmp = m_creature->SummonCreature(NPC_NECROLYTE, necrolytePos[1][0], necrolytePos[1][1], necrolytePos[1][2],
                                         0.104f, TEMPSUMMON_DEAD_DESPAWN, 0);
        leftHerosList.push_back(tmp);

        m_creature->GetMotionMaster()->MovePoint(1, guideGoPos[0], guideGoPos[1], guideGoPos[2]);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void DoAttack()
    {
        Creature *acolyte = rightHerosList[rightHerosList.size()-1];
        rightHerosList.pop_back();
        for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
            (*itr)->AI()->AttackStart(acolyte);

        acolyte = leftHerosList[leftHerosList.size()-1];
        leftHerosList.pop_back();
        for(std::vector<Creature*>::iterator itr = leftHerosList.begin(); itr != leftHerosList.end(); ++itr)
        {
            (*itr)->AI()->AttackStart(acolyte);
            // put everything into right
            rightHerosList.push_back(*itr);
        }
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if(uiMoveType != POINT_MOTION_TYPE || uiPointId != 1 || !stopped)
            return;
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        if(Creature *pKrick = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KRICK)))
            m_creature->SetOrientation(m_creature->GetAngle(pKrick));
        WorldPacket heart;
        m_creature->BuildHeartBeatMsg(&heart);
        m_creature->SendMessageToSet(&heart, false);
    }

    void DoStrangulate(bool up, uint32 time)
    {
        float x, y, z;
        for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
        {
            (*itr)->GetPosition(x, y, z);
            if(up)
            {
                (*itr)->CastSpell(*itr, SPELL_STRANGULATE, true);
                (*itr)->AI()->DoAction(0);
                (*itr)->GetMotionMaster()->Clear(false, true);
                (*itr)->GetMotionMaster()->MoveIdle();
                z += 10.0f;
            }
            else
            {
                (*itr)->RemoveAurasDueToSpell(SPELL_STRANGULATE);
                z -= 9.5f;
            }
            
            (*itr)->SendMonsterMove(x, y, z, SPLINETYPE_NORMAL, up ? SPLINEFLAG_FLYING : SPLINEFLAG_FALLING, time);
            (*itr)->GetMap()->CreatureRelocation(*itr, x, y, z, 0.104f);
        }
    }

    void SetFlying(bool set)
    {
        for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
        {
            if(set)
            {
                (*itr)->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
                (*itr)->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            }
            else
            {
                (*itr)->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
                (*itr)->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            }
            WorldPacket heart;
            (*itr)->BuildHeartBeatMsg(&heart);
            (*itr)->SendMessageToSet(&heart, false);
        }
    }

    void DoAction(uint32 action)
    {
        if(int32(action) < 0)
        {
            DoScriptText(action, m_creature);
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(stopped)
            return;
        
        if(m_uiEventTimer <= uiDiff)
        {
            switch(eventStateInternal)
            {
                case 0:
                   SetEventState(1);
                   pTyrannus->AI()->DoAction(SAY_TYRANNUS_INTRO1);
                   m_uiEventTimer = 5000;
                   break;
               case 1:
                   pTyrannus->AI()->DoAction(SAY_TYRANNUS_INTRO2);
                   m_uiEventTimer = 12500;
                   break;
               case 2:
                   DoScriptText(say_guide[0][faction], m_creature);
                   m_uiEventTimer = 1500;
                   break;
               case 3:
                   DoAttack();
                   m_uiEventTimer = 5000;
                   break;
               case 4:
                   DoStrangulate(true, 4000);
                   pTyrannus->AI()->DoAction(SAY_TYRANNUS_INTRO3);
                   m_uiEventTimer = 4000;
                   break;
               case 5:
                   SetFlying(true);
                   m_uiEventTimer = 2000;
                   break;
               case 6:
                   pTyrannus->AI()->DoAction(SOUND_TYRRANNUS_TRANSFORM);
                   m_uiEventTimer = 7000;
                   break;
               case 7:
                   for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
                   {
//                       pTyrannus->CastSpell(m_creature,  SPELL_NECROMANTIC_POWER, true);
                       (*itr)->SetStandState(UNIT_STAND_STATE_DEAD);
                   }
                   SetFlying(false);
                   DoStrangulate(false, 1000);
                   DoScriptText(say_guide[1][faction], m_creature);
                   m_uiEventTimer = 3000;
                   break;
               case 8:
                   for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
                   {
                       (*itr)->SetStandState(UNIT_STAND_STATE_STAND);
                       pTyrannus->CastSpell(*itr, SPELL_TURN_TO_UNDEAD, true);
                       (*itr)->CombatStop();
                       (*itr)->UpdateEntry(NPC_SKELETAL_SLAVE);
                   }
                   m_uiEventTimer = 2000;
                   break;
               case 9:
                   pTyrannus->AI()->DoAction(SAY_TYRANNUS_INTRO4);
                   for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
                   {
                       (*itr)->AI()->DoAction(1);
                       (*itr)->AI()->AttackStart(m_creature);
                       (*itr)->AddThreat(m_creature, 1000000.0f);
                   }
                   m_uiEventTimer = 3000;
                   break;
               case 10:
                   if(faction)
                       DoCast(m_creature, SPELL_SYLVANAS_PWNT);
                   else
                   {
                       for(std::vector<Creature*>::iterator itr = rightHerosList.begin(); itr != rightHerosList.end(); ++itr)
                           m_creature->CastSpell(*itr, SPELL_JAINA_PWNT, true);
                   }
                   m_uiEventTimer = 2000;
                   break;
               case 11:
                   DoScriptText(say_guide[2][faction], m_creature);
                   m_uiEventTimer = 4000;
                   break;
               case 12:
                   pTyrannus->AI()->DoAction(0);
                   pTyrannus->AI()->DoAction(1);
                   DoScriptText(say_guide[3][faction], m_creature);
                   m_uiEventTimer = 10000;
                   break;
               case 13:
                   stopped = true;
                   break;
               // ====================== INTRO END ===============
            }
            ++eventStateInternal;
        }else m_uiEventTimer -= uiDiff;
    }
};

const float tyrannusPos [][3] =
{
    { 842.02f, 264.17f, 620.03f },
    { 829.35f, 149.98f, 548.53f },
    { 952.54f, 164.32f, 669.85f },
};

struct MANGOS_DLL_DECL mob_tyrannus_introAI : public ScriptedAI
{
    mob_tyrannus_introAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool toInvis;
    uint32 invisTimer;

    void Reset()
    {
        invisTimer = 17000;
        toInvis = false;
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void DoAction(uint32 action)
    {
        if(int32(action) < 0)
        {
            DoScriptText(action, m_creature);
            return;
        }
        switch(action)
        {
            case SOUND_TYRRANNUS_TRANSFORM:
                DoPlaySoundToSet(m_creature, action);
                return;
            case 0:
                DoMove(0);
                break;
            case 1:
                toInvis = true;
                break;
            case 2:
                m_creature->SetVisibility(VISIBILITY_ON);
                DoMove(1);
                break;
            case 3:
                DoMove(2);
                break;
        }
    }

    void DoMove(uint8 pos)
    {
        PointPath path;
        path.resize(2);
        path.set(0, PathNode(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()));
        path.set(1, PathNode(tyrannusPos[pos][0], tyrannusPos[pos][1], tyrannusPos[pos][2]));
        uint32 time = m_creature->GetDistance(path[1].x, path[1].y, path[1].z)/(10.0f*0.001f);
        m_creature->GetMotionMaster()->MoveCharge(path, time, 1, 1);
        m_creature->SendMonsterMove(path[1].x, path[1].y, path[1].z, SPLINETYPE_NORMAL , SPLINEFLAG_FLYING, time);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(toInvis)
        {
            if(invisTimer <= uiDiff)
            {
                m_creature->SetVisibility(VISIBILITY_OFF);
                toInvis = false;
            }else invisTimer-= uiDiff;
        }
    }
};

struct MANGOS_DLL_DECL mob_pos_heroAI : public ScriptedAI
{
    mob_pos_heroAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
    }

    void EnterEvadeMode()
    {

    }
    
    void DoAction(uint32 action)
    {
        if(int32(action) < 0)
            DoScriptText(action, m_creature);
        else
            SetCombatMovement(action);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if(uiMoveType != POINT_MOTION_TYPE || m_creature->GetMapId() != 632 || uiPointId != 3)
            return;
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetVisibility(VISIBILITY_OFF);
        
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

const float sindragosaPos[4] = { 877.11, 202.16, 560.35, 5.921 };
const float spawnPos[4] = { 1070.48, 103.21, 630.73, 2.13 };
const float endNpcPos[3] = { 1003.18, 154.61, 628.2};
const float horPos[3] = { 1096.62, 241.87, 630 };

const int32 slave_says[2] = {-1658313, -1658312 };

const int32 guide_end[][2] =
{
    {-1658315, -1658314 },
    {-1658317, -1658318 },
    {-1658316, 0 },
};

const float outroPos[][3] =
{
   {995.117, 136.438, 628.156},
   {995.724, 143.097, 628.156},
   {994.550, 156.671, 628.156},
   {994.700, 160.007, 628.156},
   {995.928, 174.579, 628.156},
   {997.359, 182.870, 628.156},
   {1003.825, 122.176, 628.156},
   {1004.975, 126.011, 628.156},
   {1015.304, 145.097, 628.156},
   {1015.880, 150.601, 628.156},
   {1012.257, 163.875, 628.156},
   {1011.410, 169.595, 628.156},
   {1016.512, 191.425, 628.156},
   {1016.512, 191.425, 628.156},
   {1025.370, 160.074, 628.156},
   {1025.345, 156.223, 628.156}, // MAGE
   {1024.981, 136.201, 628.156}, // mage
   {1016.030, 125.871, 628.156},
   {1017.468, 120.017, 628.156},
   {1032.587, 147.116, 628.156},
   {1041.433, 160.520, 628.156}, // MAGE
   {1041.589, 165.289, 628.156}, // MAGE
   {1032.082, 174.175, 628.156}, // MAGE
   {1042.440, 175.693, 628.156},
   {1042.075, 181.341, 628.156},
   {1031.804, 188.817, 628.156},
};

#define OUTRO_MOB_COUNT 26

struct MANGOS_DLL_DECL mob_pos_guide_endAI : public ScriptedAI
{
    mob_pos_guide_endAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEventTimer;
    uint8 faction;
    uint8 eventState;
    uint8 eventStateInternal;
    bool stopped;

    std::vector<Creature*> herosList;

    Creature *pSindragosa;
    Creature *pEndNpc;

    void Reset()
    {
        if(!m_pInstance)
        {
            m_creature->ForcedDespawn();
            return;
        }
        faction = m_pInstance->GetData(TYPE_FACTION);
        eventState = m_pInstance->GetData(TYPE_EVENT_STATE);
        stopped = true;
        pEndNpc = m_creature->SummonCreature(faction ? NPC_END_HORDE : NPC_END_ALLI, spawnPos[0], spawnPos[1], spawnPos[2],
                                             spawnPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
        m_uiEventTimer = 5000;
        eventStateInternal = 0;
        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
     }

    void SetEventState(uint32 state)
    {
         eventState = state;
         m_pInstance->SetData(TYPE_EVENT_STATE, state);
    }

    void SpawnMobs()
    {
        Creature *tmp;
        for(uint8 i = 0; i < OUTRO_MOB_COUNT; ++i)
        {
            tmp = m_creature->SummonCreature(heroIds[urand(0, 2)][faction], spawnPos[0], spawnPos[1], spawnPos[2],
                                             spawnPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
            tmp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            tmp->GetMotionMaster()->MovePoint(1, outroPos[i][0], outroPos[i][1], outroPos[i][2]);
            herosList.push_back(tmp);
        }

        pEndNpc->GetMotionMaster()->MovePoint(1, endNpcPos[0], endNpcPos[1], endNpcPos[2]);
        pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, sindragosaPos[0], sindragosaPos[1], sindragosaPos[2],
                                                 sindragosaPos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void DoAction(uint32 action)
    {
        if(int32(action) < 0)
        {
            DoScriptText(action, m_creature);
            return;
        }

        switch(action)
        {
            case 0:
                pEndNpc->AI()->DoAction(faction ? SAY_SLAVE_INTRO_H : SAY_SLAVE_INTRO_A);
                break;
            case 1:
                SpawnMobs();
                stopped = false;
                break;
        }
    }

    void KillAll()
    {
        std::vector<Creature*> tmp = herosList;
        herosList.clear();
        for(std::vector<Creature*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
        {
            if((*itr)->GetEntry() == heroIds[2][faction])
            {
                (*itr)->CastSpell(*itr, 62766, false);
                herosList.push_back(*itr);
                continue;
            }
            (*itr)->CastSpell(*itr, 7, true);
        }
        pEndNpc->CastSpell(pEndNpc, 7, true);
    }

    void ToPortal()
    {
        for(std::vector<Creature*>::iterator itr = herosList.begin(); itr != herosList.end(); ++itr)
        {
            (*itr)->RemoveAurasDueToSpell(62766);
            (*itr)->GetMotionMaster()->MovePoint(2, horPos[0], horPos[1], horPos[2]);
            (*itr)->ForcedDespawn(15000);
        }
    }

    void DoFlySindragosa()
    {
        pSindragosa->SetSpeedRate(MOVE_FLIGHT, 4, true);
        pSindragosa->SetSpeedRate(MOVE_RUN, 4, true);
        pSindragosa->SetSpeedRate(MOVE_WALK, 4, true);

        pSindragosa->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
        pSindragosa->m_movementInfo.AddMovementFlag(MOVEFLAG_LEVITATING);
        WorldPacket heart;
        pSindragosa->BuildHeartBeatMsg(&heart);
        pSindragosa->SendMessageToSet(&heart, false);

        float x, y ,z;
        pSindragosa->GetPosition(x, y, z);
        z += 100.0f;
        x += cos(pSindragosa->GetOrientation())*35;
        y += sin(pSindragosa->GetOrientation())*35;
        
        PointPath path;
        path.resize(2);
        path.set(0, PathNode(pSindragosa->GetPositionX(), pSindragosa->GetPositionY(), pSindragosa->GetPositionZ()));
        path.set(1, PathNode(x,y,z));
        pSindragosa->GetMotionMaster()->MoveCharge(path, 2000, 1, 1);
        pSindragosa->SendMonsterMove(path[1].x, path[1].y, path[1].z, SPLINETYPE_NORMAL , SPLINEFLAG_NONE, 2000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(stopped)
            return;

        if(m_uiEventTimer <= uiDiff)
        {
            switch(eventStateInternal)
            {
                case 0:
                    pEndNpc->AI()->DoAction(slave_says[faction]);
                    m_uiEventTimer = faction ? 28000 : 24000;
                    break;
                case 1:
                    DoFlySindragosa();
                    m_uiEventTimer = 2000;
                    break;
                case 2:
                {
                    Creature *pFrostBomb = m_creature->SummonCreature(NPC_FROST_BOMB, endNpcPos[0], endNpcPos[1], endNpcPos[2], 0,
                                                                      TEMPSUMMON_TIMED_DESPAWN, 5000);
                    pSindragosa->CastSpell(pFrostBomb, SPELL_FROST_BOMB, true);
                    m_uiEventTimer = 2000;
                    break;
                }
                case 3:
                {
                    DoScriptText(guide_end[0][faction], m_creature);

                    Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                    if(!lPlayers.isEmpty())
                    {
                    
                    std::vector<Player*> plrlist;
                    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        if (Player* pPlayer = itr->getSource())
                            plrlist.push_back(pPlayer);
                    for(std::vector<Player*>::iterator itr = plrlist.begin(); itr != plrlist.end(); ++itr)
                    {
                            if(!(*itr) || !(*itr)->IsInWorld() || !(*itr)->isAlive())
                            continue;
                            (*itr)->CastSpell(*itr, faction ? SPELL_TELE_HORDE : SPELL_TELE_ALI, true);
                    }
                    }
                    KillAll();
                    m_uiEventTimer = 3000;
                    break;
                }
                case 4:
                {
                    pSindragosa->GetMotionMaster()->MovePoint(2, tyrannusPos[0][0], tyrannusPos[0][1], tyrannusPos[0][2]);
                    DoScriptText(guide_end[1][faction], m_creature);
                    m_uiEventTimer = faction? 13000 : 7000;
                    break;
                }
                case 5:
                {
                    if(faction)
                        break;
                    DoScriptText(guide_end[2][faction], m_creature);
                    m_uiEventTimer = 7000;
                    break;
                }
                case 6:
                    m_creature->GetMotionMaster()->MovePoint(2, horPos[0], horPos[1], horPos[2]);
                    pSindragosa->SetVisibility(VISIBILITY_OFF);
                    ToPortal();
                    stopped = true;
                    break;
            }
            ++eventStateInternal;
        }else m_uiEventTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_pos_guide_start(Creature* pCreature)
{
    return new mob_pos_guide_startAI(pCreature);
}

CreatureAI* GetAI_mob_tyrannus_intro(Creature* pCreature)
{
    return new mob_tyrannus_introAI(pCreature);
}

CreatureAI* GetAI_mob_pos_hero(Creature* pCreature)
{
    return new mob_pos_heroAI(pCreature);
}

CreatureAI* GetAI_mob_pos_guide_end(Creature* pCreature)
{
    return new mob_pos_guide_endAI(pCreature);
}

void AddSC_pit_of_saron()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mob_pos_guide_start";
    newscript->GetAI = &GetAI_mob_pos_guide_start;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_pos_guide_end";
    newscript->GetAI = &GetAI_mob_pos_guide_end;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tyrannus_intro";
    newscript->GetAI = &GetAI_mob_tyrannus_intro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_pos_hero";
    newscript->GetAI = &GetAI_mob_pos_hero;
    newscript->RegisterSelf();
}