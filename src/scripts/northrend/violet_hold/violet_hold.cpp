#include "precompiled.h"
#include "escort_ai.h"
#include "TemporarySummon.h"
#include "violethold.h"
#include "violet_hold_invaderAI.h"

const float portals[8][4]=
{
    {1936.07f, 803.198f, 53.3749f, 3.12414f},
    {1877.51f, 850.104f, 44.6599f, 4.7822f}, 
    {1890.64f, 753.471f, 48.7224f, 1.71042f},
    {1857.89f, 769.447f, 38.6508f, 0.80225f},
    {1902.71f, 833.180f, 38.7790f, 4.14123f},
    {1911.04f, 805.779f, 38.6454f, 3.19500f},
    {1918.02f, 853.677f, 47.1628f, 4.24744f},
    {1930.09f, 760.388f, 51.2804f, 2.38996f}
};

const float guards[4][4]=
{
    {1850.694f, 798.112f, 44.022f, 0.385f},
    {1849.572f, 801.119f, 44.043f, 0.204f},
    {1849.135f, 806.254f, 44.056f, 6.185f},
    {1850.268f, 809.763f, 44.029f, 6.238f},
};

const float GuardsCompleted[2][3]=
{
    {1827.53f, 799.994f, 44.36331f},
    {1827.18f, 807.778f, 44.41047f}
};

const float sinclari[4]=
{
    1830.95f, 799.463f, 44.418f, 2.3911f
};

const float BossPortal[4]=
{
    1888.064f, 803.225f, 38.353f, 6.227f
};

const float CyanigosaPortal[2][4]= // portal + her coordinates
{
    /*{1936.07f, 803.198f, 53.3749f, 3.12414f},
    {1930.07f, 803.198f, 53.3749f, 3.12414f} upper, no jump -> spawn her down instead*/
    {1905.40f, 803.012f, 38.64513f, 3.12414f},
    {1896.11f, 803.063f, 38.49596f, 3.12414f}
};

const float SinclariWP[5][4]=
{
    {1828.918f, 798.474f, 44.363f, 3000},
    {1831.997f, 798.390f, 44.318f, 3000},
    {1817.266f, 803.919f, 44.363f, 0},
    {1819.069f, 803.963f, 44.365f, 12000},
    {1816.164f, 804.037f, 44.365f, 0},
};

const float finishWP[2][3]=
{
    {1836.821f, 803.990f, 44.243f},
    {1801.385f, 804.238f, 44.363f},
};

const float TeleIn[4]={1827.29f, 803.932f, 44.36348f, 0};

const uint8 NumberOfWaypoints[PRISONBOSSES]={2, 4, 7, 4, 3, 2};//lavanthor, moragg, zulamat, erekem, ichiron, xevozz,

const float LavanthorReleaseWP[2][3]=
{
    {1887.967f, 797.373f, 38.475f},
    {1855.370f, 761.252f, 38.655f},
};

const float MoraggReleaseWP[4][3]=
{
    {1887.967f, 797.373f, 38.475f},
    {1889.844f, 777.014f, 38.917f},
    {1891.073f, 764.246f, 47.502f},
    {1894.645f, 739.633f, 47.667f},
};

const float ZuramatReleaseWP[7][3]=
{
    {1887.65f, 808.86f, 38.406f},
    {1912.81f, 821.19f, 38.681f},
    {1926.19f, 826.70f, 43.951f},
    {1929.25f, 831.97f, 45.501f},
    {1928.62f, 837.34f, 47.031f},
    {1924.90f, 846.31f, 47.155f},
    {1926.68f, 850.97f, 47.185f},
};

const float ErekemReleaseWP[4][3]=
{
    {1887.65f, 808.86f, 38.406f},
    {1880.67f, 835.27f, 38.956f},
    {1879.37f, 841.97f, 43.334f},
    {1873.77f, 864.78f, 43.410f}
};

const float IchoronReleaseWP[3][3]=
{
    {1887.967f, 797.373f, 38.475f},
    {1903.181f, 791.396f, 38.647f},
    {1910.053f, 784.551f, 37.540f}
};

const float XevozzReleaseWP[2][3]=
{
    {1887.65f, 808.86f, 38.406f},
    {1905.64f, 840.81f, 38.660f},
};

#define S_AGONIZINGSTRIKE 58504

struct MANGOS_DLL_DECL mob_portalguardianAI : public npc_escortAI
{
    mob_portalguardianAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint64 MotherPortal;

    uint16 AgonizingStrikeTimer;
    uint8 MotherPortalNumber;

    void Reset()
    {
        AgonizingStrikeTimer=rand()%8000;
        MotherPortal=0;
    }

    void WaypointReached(uint32 point)
    {
    }

    void JustDied(Unit* killer)
    {
        if (MotherPortal)
        {
            Creature* tempcreature=m_creature->GetMap()->GetCreature(MotherPortal);
            if (tempcreature)
                tempcreature->AI()->DoAction(MODIFIER+1);
        }
    }

    void DoAction(uint32 action)
    {
        MotherPortalNumber=action;
    }
        

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (AgonizingStrikeTimer<=diff)
        {
            m_creature->CastSpell(m_creature->getVictim(),S_AGONIZINGSTRIKE,false);
            AgonizingStrikeTimer=8000+rand()%5000;
        }
        else AgonizingStrikeTimer-=diff;
        
        DoMeleeAttackIfReady();
    }
};

#define S_KEEPERFROSTBOLT 58535
#define S_KEEPERFROSTBOLT_H 61590
#define S_KEEPERARCANEVOLLEY 58536
#define S_KEEPERARCANEVOLLEY_H 61591

struct MANGOS_DLL_DECL mob_portalkeeperAI : public npc_escortAI
{
    mob_portalkeeperAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    bool m_bIsRegularMode;

    uint64 MotherPortal;

    uint16 FrostboltTimer;
    uint16 ArcaneVolleyTimer;

    void Reset()
    {
        FrostboltTimer=rand()%9000;
        ArcaneVolleyTimer=rand()%20000;
        MotherPortal=0;
    }

    void WaypointReached(uint32 point)
    {
    }

    void JustDied(Unit* killer)
    {
        if (MotherPortal)
        {
            Creature* tempcreature=m_creature->GetMap()->GetCreature(MotherPortal);
            if (tempcreature)
                tempcreature->AI()->DoAction(MODIFIER+1);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (FrostboltTimer<=diff)
        {
            m_creature->CastSpell(m_creature->getVictim(),m_bIsRegularMode ? S_KEEPERFROSTBOLT : S_KEEPERFROSTBOLT_H,false);
            FrostboltTimer=3000+rand()%6000;
        }
        else FrostboltTimer-=diff;

        if (ArcaneVolleyTimer<=diff)
        {
            m_creature->CastSpell(m_creature->getVictim(),m_bIsRegularMode ? S_KEEPERARCANEVOLLEY : S_KEEPERARCANEVOLLEY_H,false);
            ArcaneVolleyTimer=10000+rand()%10000;
        }
        else ArcaneVolleyTimer-=diff;
        
        DoMeleeAttackIfReady();
    }
};

#define S_WHRILWIND_CAPTAIN         41056
#define S_MORTAL_STRIKE_CAPTAIN     32736

struct MANGOS_DLL_DECL mob_azurecaptainAI : public violet_hold_squadAI
{
    mob_azurecaptainAI(Creature* pCreature) : violet_hold_squadAI(pCreature)
    {
        Reset();
    }

    uint16 WhrilwindTimer;
    uint16 MortalStrikeTimer;
    
    void Reset()
    {
        violet_hold_squadAI::Reset();
        WhrilwindTimer=0;
        MortalStrikeTimer=urand(3000,6000);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_invaderAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (WhrilwindTimer<diff)
        {
            DoCast(m_creature,S_WHRILWIND_CAPTAIN);
            WhrilwindTimer=urand(5000,9000);
        }
        else WhrilwindTimer-=diff;

        if (MortalStrikeTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_MORTAL_STRIKE_CAPTAIN);
            MortalStrikeTimer=urand(7000,10000);
        }
        else MortalStrikeTimer-=diff;

        DoMeleeAttackIfReady();
    }
};

#define S_CONCUSSION_BLOW_RAIDER        52719
#define S_MAGIC_REFLECTION              60158

struct MANGOS_DLL_DECL mob_azureraiderAI : public violet_hold_squadAI
{
    mob_azureraiderAI(Creature* pCreature) : violet_hold_squadAI(pCreature)
    {
        Reset();
    }

    uint16 ReflectionTimer;
    uint16 ConcussionBlowTimer;

    void Reset()
    {
        violet_hold_squadAI::Reset();
        ConcussionBlowTimer=0;
        ReflectionTimer=urand(7000,14000);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_squadAI::UpdateEscortAI(diff);
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (ConcussionBlowTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_CONCUSSION_BLOW_RAIDER);
            ConcussionBlowTimer=urand(9000,14000);
        }
        else ConcussionBlowTimer-=diff;

        if (ReflectionTimer<diff)
        {
            DoCast(m_creature,S_MAGIC_REFLECTION);
            ReflectionTimer=urand(12000,18000);
        }
        else ReflectionTimer-=diff;

        DoMeleeAttackIfReady();
    }
};

#define S_MANA_DETONATION_SORCEROR          60182
#define S_MANA_DETONATION_SORCEROR_H        60205
#define S_ARCANE_STREAM_SORCEROR            60181
#define S_ARCANE_STREAM_SORCEROR_H          60204

struct MANGOS_DLL_DECL mob_azuresorcerorAI : public violet_hold_squadAI
{
    mob_azuresorcerorAI(Creature* pCreature) : violet_hold_squadAI(pCreature)
    {
        Reset();
    }
    
    uint16 MovingUpdateTimer;
    bool moving;
    uint16 ArcaneStreamTimer;
    uint16 ManaDetonationTimer;

    void Reset()
    {
        violet_hold_squadAI::Reset();
        MovingUpdateTimer=1000;
        moving=true;
        ArcaneStreamTimer=3000;
        ManaDetonationTimer=1000;
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_squadAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (MovingUpdateTimer)
        {
            if (MovingUpdateTimer<=diff)
            {
                MovingUpdateTimer=500;
                if ((m_creature->GetDistance(m_creature->getVictim())<=35.0f) && (m_creature->GetDistance(m_creature->getVictim())>=5.0f))
                {
                    moving=false;
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->StopMoving();
                }
                else
                {
                    moving=true;
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }

                if (m_creature->GetPower(POWER_MANA)<7)
                {
                    moving=true;
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    MovingUpdateTimer=0;
                }
            }
            else MovingUpdateTimer-=diff;
        }

        if (moving)
        {
            if (ManaDetonationTimer<diff)
            {
                DoCast(m_creature,m_bIsRegularMode ? S_MANA_DETONATION_SORCEROR : S_MANA_DETONATION_SORCEROR_H);
                ManaDetonationTimer=urand(9000,15000);
            }
            else ManaDetonationTimer-=diff;
            
            DoMeleeAttackIfReady();
        }
        else
        {
            if (ArcaneStreamTimer<diff)
            {
                DoCast(m_creature->getVictim(),m_bIsRegularMode ? S_ARCANE_STREAM_SORCEROR : S_ARCANE_STREAM_SORCEROR_H);
                ArcaneStreamTimer=urand(4000,5000);
            }
            else ArcaneStreamTimer-=diff;
        }
    }    
};

#define S_BACKSTAB_STALKER          58471
#define S_TACTICAL_BLINK_STALKER    58470

struct MANGOS_DLL_DECL mob_azurestalkerAI : public violet_hold_squadAI
{
    mob_azurestalkerAI(Creature* pCreature) : violet_hold_squadAI(pCreature)
    {
        Reset();
    }

    uint16 BlinkTimer;

    void Reset()
    {
        violet_hold_squadAI::Reset();
        BlinkTimer=urand(10000,12000);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_squadAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (BlinkTimer<diff)
        {
            Unit* BlinkedTarget=m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (BlinkedTarget)
            {
                DoCast(BlinkedTarget,S_TACTICAL_BLINK_STALKER);
                BlinkTimer=urand(11000,15000);
                DoCast(m_creature,S_BACKSTAB_STALKER);
            }
            BlinkTimer=urand(10000,15000);
        }

        DoMeleeAttackIfReady();
    }
};

#define SAY_NEWPORTALKEEPER -1608050
#define SAY_NEWPORTALGUARDIAN -1608051
#define SAY_NEWSQUAD -1608052

struct MANGOS_DLL_DECL npc_violetholdportalAI : public Scripted_NoMovementAI
{
    npc_violetholdportalAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
          Reset();
          m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_instance;
    uint64 MainCreature[4];
    uint8 PortalNumber;    
    uint32 SummonTimer;    
    uint16 DespawnTimer;
    bool EventActive;
    bool FirstSpawn;
    bool isBoss;
    
    void Reset()
    {
        PortalNumber=0;
        EventActive=false;
        SummonTimer=15000;
        DespawnTimer=0;
        isBoss=false;
        for (uint8 i=0;i<4;i++)
            MainCreature[i]=0;
    }

    void SpawnGuardian()
    {
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        Creature* tempcreature=m_creature->SummonCreature(C_GUARDIAN,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN,120000);
        if (tempcreature)
        {                            
            MainCreature[0]=tempcreature->GetGUID();
            tempcreature->AI()->DoAction(PortalNumber);
            m_creature->CastSpell(tempcreature,S_PORTALCHARGE,true);
            ((mob_portalguardianAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        DoScriptText(SAY_NEWPORTALGUARDIAN,tempcreature,i->getSource());
                }   
            }   
        }
    }

    void SpawnKeeper()
    {
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        Creature* tempcreature=m_creature->SummonCreature(C_KEEPER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN,120000);
        if (tempcreature)
        {                            
            MainCreature[0]=tempcreature->GetGUID();
            tempcreature->AI()->DoAction(PortalNumber);
            m_creature->CastSpell(tempcreature,S_PORTALCHARGE,true);
            ((mob_portalkeeperAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        DoScriptText(SAY_NEWPORTALKEEPER,tempcreature,i->getSource());
                }
            }
        }
    }

    void SpawnSquad()
    {
        SummonTimer=(uint32)-1;
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        Creature* tempcreature=NULL;
        if (m_instance)
        {
            if (m_instance->GetData(DATA_PORTALCOUNTER)<12)
            {
                uint8 rnd=rand()%4;
                switch (rnd)
                {
                case 0:
                    tempcreature=m_creature->SummonCreature(C_STALKER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[0]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(FIRST_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_RAIDER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[1]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(SECOND_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_SORCEROR,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[2]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(THIRD_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    }
                    break;
                case 1:
                    tempcreature=m_creature->SummonCreature(C_CAPTAIN,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[0]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(FIRST_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_RAIDER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[1]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(SECOND_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_SORCEROR,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[2]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(THIRD_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    }
                    break;
                case 2:
                    tempcreature=m_creature->SummonCreature(C_CAPTAIN,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[0]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(FIRST_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_STALKER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[1]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(SECOND_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_SORCEROR,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[2]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(THIRD_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    }
                    break;
                case 3:
                    tempcreature=m_creature->SummonCreature(C_CAPTAIN,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[0]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(FIRST_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_RAIDER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[1]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(SECOND_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                        tempcreature=NULL;
                    }
                    tempcreature=m_creature->SummonCreature(C_STALKER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                    if (tempcreature)
                    {
                        MainCreature[2]=tempcreature->GetGUID();
                        tempcreature->AI()->DoAction(PortalNumber);
                        tempcreature->AI()->DoAction(THIRD_SQUAD);
                        ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    }
                }
            }
            else
            {
                tempcreature=m_creature->SummonCreature(C_CAPTAIN,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                if (tempcreature)
                {
                    MainCreature[0]=tempcreature->GetGUID();
                    tempcreature->AI()->DoAction(PortalNumber);
                    tempcreature->AI()->DoAction(FIRST_SQUAD);
                    ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    tempcreature=NULL;
                }
                tempcreature=m_creature->SummonCreature(C_RAIDER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                if (tempcreature)
                {
                    MainCreature[1]=tempcreature->GetGUID();
                    tempcreature->AI()->DoAction(PortalNumber);
                    tempcreature->AI()->DoAction(SECOND_SQUAD);
                    ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    tempcreature=NULL;
                }
                tempcreature=m_creature->SummonCreature(C_SORCEROR,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                if (tempcreature)
                {
                    MainCreature[2]=tempcreature->GetGUID();
                    tempcreature->AI()->DoAction(PortalNumber);
                    tempcreature->AI()->DoAction(THIRD_SQUAD);
                    ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                    tempcreature=NULL;
                }
                tempcreature=m_creature->SummonCreature(C_STALKER,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_DESPAWN,120000);
                if (tempcreature)
                {
                    MainCreature[3]=tempcreature->GetGUID();
                    tempcreature->AI()->DoAction(PortalNumber);
                    tempcreature->AI()->DoAction(FOURTH_SQUAD);
                    ((mob_azurecaptainAI*)tempcreature->AI())->MotherPortal=m_creature->GetGUID();
                }
            }
            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        DoScriptText(SAY_NEWSQUAD,tempcreature,i->getSource());
                }
            }
            m_creature->SetVisibility(VISIBILITY_OFF);
        }
    }

    void DoAction(uint32 action)
    {
        if (action==CYANIGOSA)
        {
            DespawnTimer=10000;
            SummonTimer=30000;
            return;
        }

        if (action>SQUAD_LIMIT)
        {
            if ((action-FIRST_SQUAD)>3)
                return;
            MainCreature[action-FIRST_SQUAD]=0;
            if ((!MainCreature[0])&&(!MainCreature[1])&&(!MainCreature[2])&&(!MainCreature[3]))
                DespawnTimer=1000;
            return;
        }
        
        if (action==BOSS)
        {
            SummonTimer=5000;
            DespawnTimer=10000;
            isBoss=true;
            return;
        }
        
        if (action>MODIFIER)
        {
            MainCreature[action-MODIFIER-1]=0;
            DespawnTimer=1000;
            return;
        }

        if (action>(2*NUMBEROFPORTALS)) 
        {
            EventActive=true;
            SummonTimer=10000;
            PortalNumber=action-(2*NUMBEROFPORTALS);
            FirstSpawn=true;
            return;
            
        }
        if (action>NUMBEROFPORTALS)
        {
            EventActive=true;
            SummonTimer=8000;
            PortalNumber=action-(NUMBEROFPORTALS);
            FirstSpawn=true;
            return;
        }

        PortalNumber=action;
    }

    void UpdateAI(const uint32 diff)
    {
        if (isBoss)
        {
            if (SummonTimer)
            {
                if(SummonTimer<=diff)
                {
                    float x,y,z;
                    m_creature->GetPosition(x,y,z);
                    if (Creature* pCreature=m_creature->SummonCreature(C_AZURESABOTEUR,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_MANUAL_DESPAWN,0))
                        SummonTimer=0;
                }
                else SummonTimer-=diff;
            }

            if (DespawnTimer<=diff)
            {
                ((TemporarySummon*)m_creature)->UnSummon();
            }
            else DespawnTimer-=diff;
            return;
        }
        
        if (!EventActive)
        {
            if (SummonTimer<=diff)
            {
                float x,y,z;
                Creature* tempcreature=NULL;
                m_creature->GetPosition(x,y,z);
                uint8 rnd=rand()%4;
                switch (rnd)
                {
                case 0:
                    if (tempcreature=m_creature->SummonCreature(C_AZUREBINDER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                        tempcreature->AI()->DoAction(PortalNumber);
                    break;
                case 1:
                    if (tempcreature=m_creature->SummonCreature(C_AZUREINVADER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                        tempcreature->AI()->DoAction(PortalNumber);
                    break;
                case 2:
                    if (tempcreature=m_creature->SummonCreature(C_AZURESPELLBREAKER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                        tempcreature->AI()->DoAction(PortalNumber);
                    break;
                case 3:
                    if (tempcreature=m_creature->SummonCreature(C_AZUREMAGESLAYER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                        tempcreature->AI()->DoAction(PortalNumber);
                    break;
                }
                SummonTimer=12000;
            }
            else SummonTimer-=diff;
        }
        else
        {
            if (SummonTimer<=diff)
            {
                SummonTimer=10000;
                if (FirstSpawn)
                {
                    uint8 rnd;
                    switch (PortalNumber)
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:                        
                        rnd=rand()%2;
                        if (!rnd)
                            SpawnGuardian();
                        else
                            SpawnKeeper();
                        break;
                    case 6:
                        rnd=rand()%3;
                        switch (rnd)
                        {
                        case 0:
                            SpawnGuardian();
                            break;
                        case 1:
                            SpawnKeeper();
                            break;
                        case 2:
                            SpawnSquad();
                            break;
                        }
                        break;
                    case 7:
                    case 8:
                        SpawnSquad();
                        break;
                    }
                    FirstSpawn=false;
                }
                else
                {
                    if (m_creature->GetVisibility())
                    {
                        float x,y,z;
                        Creature* tempcreature=NULL;
                        m_creature->GetPosition(x,y,z);
                        uint8 rnd=rand()%4;
                        switch (rnd)
                        {
                        case 0:
                            if (tempcreature=m_creature->SummonCreature(C_AZUREBINDER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                                tempcreature->AI()->DoAction(PortalNumber+NUMBEROFPORTALS);
                            break;
                        case 1:
                            if (tempcreature=m_creature->SummonCreature(C_AZUREINVADER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                                tempcreature->AI()->DoAction(PortalNumber+NUMBEROFPORTALS);
                            break;
                        case 2:
                            if (tempcreature=m_creature->SummonCreature(C_AZURESPELLBREAKER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                                tempcreature->AI()->DoAction(PortalNumber+NUMBEROFPORTALS);
                            break;
                        case 3: 
                            if (tempcreature=m_creature->SummonCreature(C_AZUREMAGESLAYER,x,y,z,0,TEMPSUMMON_CORPSE_DESPAWN,30000))
                                tempcreature->AI()->DoAction(PortalNumber+NUMBEROFPORTALS);
                            break;
                        }
                    }
                }
                SummonTimer=10000;
            }
            else SummonTimer-=diff;

            if (DespawnTimer)
            {
                if (DespawnTimer<=diff)
                {
                    if (m_instance)
                    {
                        Creature* tempcreature=m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID));
                        if (tempcreature)
                            tempcreature->AI()->DoAction(PortalNumber-1);

                    }                        
                    DespawnTimer=50000;
                }
                else DespawnTimer-=diff;
            }
        }
    }
};

struct MANGOS_DLL_DECL npc_sinclariAI : public npc_escortAI
{
    npc_sinclariAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
        m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_instance;
    bool EventActive;
    bool WaypointSet;
    uint16 StanceTimer;
    uint16 LockTimer;
    uint16 SayTimer;
    uint16 RotateTimer;

    void EventActivate()
    {
        EventActive=true;
        SetDespawnAtEnd(false);
        Start();
        if (m_instance)
        {
            Creature* Door=m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID));
            if (Door)
                Door->AI()->DoAction(EVENT_START);
        }
    }        

    void Reset()
    {
        WaypointSet=false;
        EventActive=false;
        m_instance=NULL;
        StanceTimer=0;
        LockTimer=0;
        SayTimer=0;
        RotateTimer=0;        
    }

    void WaypointReached(uint32 point)
    {
        switch (point)
        {
        case 0:
            StanceTimer=2700;
            m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE,EMOTE_STATE_USESTANDING);
            break;
        case 1:
            m_creature->MonsterYell("Prison guards, we are leaving! These adventurers are taking over! Go go go!",0,m_creature->GetGUID());
            if (m_instance)
                m_instance->SetData(DATA_DEFSYSTEM_TRIGGER,0);
            break;
        case 2:
            RotateTimer=1000;
        case 4:
            m_creature->SetFlag(UNIT_NPC_FLAGS,1);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!WaypointSet)
        {
            for (uint8 i=0;i<5;i++)
                AddWaypoint(i,SinclariWP[i][0],SinclariWP[i][1],SinclariWP[i][2],SinclariWP[i][3]);
            WaypointSet=true;
        }

        if (StanceTimer)
        {
            if (StanceTimer<=diff)
            {
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE,EMOTE_STATE_NONE);
                StanceTimer=0;
            }
            else StanceTimer-=diff;
        }

        if (RotateTimer)
        {
            if (RotateTimer<=diff)
            {
               m_creature->SetOrientation(0.0f);
               RotateTimer=0;
               SayTimer=5000;
               if (m_instance)
                {
                    Creature* Door=m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID));
                    if (Door)
                        Door->AI()->DoAction(GUARDS_ESCORT);
                }
            }
            else RotateTimer-=diff;
        }

        if (SayTimer)
        {
            if (SayTimer<=diff)
            {
                m_creature->MonsterSay("I'm locking the door. Good luck, and thank you for doing this.",0,m_creature->GetGUID());
                SayTimer=0;
                LockTimer=5000;
            }
            else SayTimer-=diff;
        }

        if (LockTimer)
        {
            if (LockTimer<=diff)
            {
                if(m_instance)
                {
                    Creature* Door=m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID));
                    if (Door)
                        Door->AI()->DoAction(CLOSE_DOOR);
                }
                LockTimer=0;
            }
            else LockTimer-=diff;
        }


        npc_escortAI::UpdateAI(diff);
    }
};

#define C_CYANIGOSA 31134

struct MANGOS_DLL_DECL npc_violetholddoorAI : public Scripted_NoMovementAI
{
    npc_violetholddoorAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {  
        Reset();
        m_instance=(ScriptedInstance*)pCreature->GetInstanceData();
    }    
    
    ScriptedInstance* m_instance;
    bool EventActive;
    uint64 Portal[NUMBEROFPORTALS+1];
    uint64 Guard[4];
    uint64 Sinclari;
    uint16 PortalsDespawnTimer;
    uint32 PortalSpawnTimer;
    uint16 ResetTimer;
    uint8 lastportal;
    uint8 NumberOfPortals;
    uint8 PortalCounter;
    bool FirstPortal;
    bool Spawned;
    bool isBoss;

    void Reset()
    {
        EventActive=false;
        ResetTimer=0;
        Spawned=0;
        PortalsDespawnTimer=0;
        PortalSpawnTimer=0;
        lastportal=0;
        Sinclari=0;
        NumberOfPortals=0;
        PortalCounter=0;
        FirstPortal=false;
        isBoss=false;
        for(int i=0;i<=NUMBEROFPORTALS;i++)
            Portal[i]=0;
        for(int i=0;i<4;i++)
            Guard[i]=0;
    }

    bool CanDestroyDoor(Unit* who)
    {
        uint32 entry;
        entry=who->GetEntry();
        if (!(entry==C_STALKER || entry==C_RAIDER || entry==C_SORCEROR || entry==C_CAPTAIN || entry==C_AZUREBINDER 
            || entry==C_AZUREINVADER  || entry==C_AZURESPELLBREAKER || entry== C_AZUREMAGESLAYER))
            return false;
        if (!(who->isAlive()))
            return false;
        if (!(who->IsWithinDistInMap(m_creature,20.0f)))
            return false;
        if (who->getVictim())
            return false;
        else return true;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (EventActive && (!FirstPortal))
        {
            if (CanDestroyDoor(who))
            {
                who->StopMoving();
                ((npc_escortAI*)(((Creature*)who)->AI()))->SetEscortPaused(true);
                who->CastSpell(m_creature,S_DESTROYDOORSEAL,false);
                ((Creature*)who)->AI()->DoAction(ATTACKINGDOOR);
            }
        }
        Scripted_NoMovementAI::MoveInLineOfSight(who);
    }

    void DoAction(uint32 action)
    {
        GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature,191723,1000.0f);
        switch(action)
        {
        case CYANIGOSA:
            if (pDoor)
                pDoor->SetGoState(GO_STATE_ACTIVE);
            if (Creature* pSinclari = m_creature->GetMap()->GetCreature(Sinclari))
            {
                pSinclari->GetMotionMaster()->MovePoint(0,m_creature->GetPositionX(), m_creature->GetPositionY(), pSinclari->GetPositionZ());
                pSinclari->MonsterSay("You did it! You held the Blie Dragonflight back and defeated tehir commander. Amazing work!",0,pSinclari->GetGUID());
            }
            for (uint8 i=0;i<2;i++)
                m_creature->SummonCreature(C_GUARD,GuardsCompleted[i][0],GuardsCompleted[i][1],GuardsCompleted[i][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
            break;
        case BOSS_DEAD:
            PortalSpawnTimer=35000;
            isBoss=false;
            if(PortalCounter <= 6)
                ((InstanceMap*)m_instance->instance)->KilledCreature("First Prisoner");
            else if(PortalCounter <= 12)
                ((InstanceMap*)m_instance->instance)->KilledCreature("Second Prisoner");
            break;
        case EVENT_START: //start event
            m_instance->SetData(DATA_CANWIPEAGAIN,0);
            EventActive=true;
            PortalsDespawnTimer=8000;
            FirstPortal=true;
            PortalSpawnTimer=35000;            
            break; 
        case CLOSE_DOOR:
            if (pDoor)
                pDoor->SetGoState(GO_STATE_READY);
            break;
        case GUARDS_ESCORT:
            for (uint8 i=0;i<4;i++)
            {
                if (Guard[i])
                {
                    if (m_creature->GetMap()->GetCreature(Guard[i]))
                        m_creature->GetMap()->GetCreature(Guard[i])->AI()->DoAction(12345);                                        
                }
            }
            break;
        case DESPAWN_WIPE:
            for (uint8 i=0;i<(NUMBEROFPORTALS+1);i++)
            {
                if (Portal[i])
                {
                    Creature* tempcreature=(Creature*)Unit::GetUnit(*m_creature,Portal[i]);
                    if (tempcreature)
                    {
                        ((TemporarySummon*)tempcreature)->UnSummon();
                        Portal[i]=0;
                    }
                }
            }
            if (Sinclari)
            {
                Creature* tempcreature=(Creature*)Unit::GetUnit(*m_creature,Sinclari);
                if (tempcreature)
                {
                    ((TemporarySummon*)tempcreature)->UnSummon();
                    Sinclari=0;
                }
            }
            for (uint8 i=0;i<4;i++)
            {
                if (Guard[i])
                {
                    if (Creature* pGuard = m_creature->GetMap()->GetCreature(Guard[i]))
                    {
                        ((TemporarySummon*)pGuard)->UnSummon();
                        Guard[i]=0;
                    }
                }
            }
            if (pDoor)
                pDoor->SetGoState(GO_STATE_ACTIVE);
            ResetTimer=60000;
            break;
        default: 
            if (Portal[action])
            {
                Creature* tempcreature=(Creature*)Unit::GetUnit(*m_creature,Portal[action]);
                if (tempcreature)
                {
                    ((TemporarySummon*)tempcreature)->UnSummon();
                    Portal[action]=0;
                    NumberOfPortals--;
                    if (!NumberOfPortals && !isBoss)
                    {
                        if (PortalSpawnTimer>3000)
                            PortalSpawnTimer=3000;
                    }
                }
            }
        }
    }
    
    void UpdateAI(const uint32 diff)
    {
        if (ResetTimer)
        {
            if (ResetTimer<=diff)
                Reset();
            else ResetTimer-=diff;
            return;
        }        
        
        if (!Spawned)
        {
            if (m_instance && m_instance->GetData(DATA_COMPLETED))
            {
                m_creature->SummonCreature(C_SINCLARI,m_creature->GetPositionX(), m_creature->GetPositionY(),GuardsCompleted[0][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
                for (uint8 i=0;i<2;i++)
                    m_creature->SummonCreature(C_GUARD,GuardsCompleted[i][0],GuardsCompleted[i][1],GuardsCompleted[i][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
                Spawned=true;
                return;
            }
            

            for (int i=0;i<3;i++)
            {
                Creature* tempcreature = m_creature->SummonCreature(C_PORTAL,portals[i][0],portals[i][1],portals[i][2],portals[i][3],TEMPSUMMON_MANUAL_DESPAWN,0);
                if (tempcreature) 
                {
                    Portal[i]=tempcreature->GetGUID();
                    tempcreature->AI()->DoAction(i+1);
                }
            }
    
            for (int i=0;i<4;i++)
            {
                Creature* tempcreature = m_creature->SummonCreature(C_GUARD,guards[i][0],guards[i][1],guards[i][2],guards[i][3],TEMPSUMMON_MANUAL_DESPAWN,0);
                if (tempcreature)
                    Guard[i]=tempcreature->GetGUID();
            }
    
            Creature* tempcreature=m_creature->SummonCreature(C_SINCLARI,sinclari[0],sinclari[1],sinclari[2],sinclari[3],TEMPSUMMON_MANUAL_DESPAWN,0);
            if (tempcreature)
                Sinclari=tempcreature->GetGUID();
            Spawned=true;
        }
        
        if (!EventActive)
            return;

        if (PortalsDespawnTimer)
        {
            if (PortalsDespawnTimer<=diff)
            {
                for (uint8 i=0;i<NUMBEROFPORTALS;i++)
                {
                    if (Portal[i])
                    {
                        Creature* tempcreature=(Creature*)Unit::GetUnit(*m_creature,Portal[i]);
                        if (tempcreature)
                        {
                            ((TemporarySummon*)tempcreature)->UnSummon();
                            Portal[i]=0;
                        }
                    }
                }
                PortalsDespawnTimer=0;
            }
            else PortalsDespawnTimer-=diff;
        }

        if (PortalSpawnTimer<=diff)
        {
            if (!PortalCounter)
            {
				Map* pMap = m_creature->GetMap();
		        if (pMap && pMap->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = pMap->GetPlayers();
                    if (!PlayerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            ((Player*)i->getSource())->SendUpdateWorldState(3816,1);
                    }   
                }
            }
            if ((PortalCounter!=5)&&(PortalCounter!=11)&&(PortalCounter!=17))
            {
                uint8 rnd=rand()%NUMBEROFPORTALS;
                if ((!Portal[rnd])&&(rnd-lastportal))
                {
                    Creature* tempcreature=m_creature->SummonCreature(C_PORTAL,portals[rnd][0],portals[rnd][1],portals[rnd][2],portals[rnd][3],TEMPSUMMON_MANUAL_DESPAWN,0);   
                    if (tempcreature)
                    {
                        Portal[rnd]=tempcreature->GetGUID();
                        if (FirstPortal)
                        {
                            (tempcreature)->AI()->DoAction((NUMBEROFPORTALS*2)+rnd+1);
                            FirstPortal=false;
                        }
                        else 
                            tempcreature->AI()->DoAction(NUMBEROFPORTALS+rnd+1);
                    }                    
                    lastportal=rnd;
                    PortalSpawnTimer=120000;
                    NumberOfPortals++;
                    PortalCounter++;
                    m_instance->SetData(DATA_PORTALCOUNTER,PortalCounter);
                }
            }
            else
            {
                if(PortalCounter==17)
                {
                    if (Creature* Portal = m_creature->SummonCreature(C_PORTAL,CyanigosaPortal[0][0],CyanigosaPortal[0][1],CyanigosaPortal[0][2],CyanigosaPortal[0][3],TEMPSUMMON_TIMED_DESPAWN,10000))
                        Portal->AI()->DoAction(CYANIGOSA);
                    if (Creature* Cyanigosa = m_creature->SummonCreature(C_CYANIGOSA,CyanigosaPortal[1][0],CyanigosaPortal[1][1],CyanigosaPortal[1][2],CyanigosaPortal[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000))
                    {
                        Cyanigosa->AI()->DoAction(BOSS_PULL);
                    }
                    PortalSpawnTimer=(uint32)-1;
                    PortalCounter++;
                    m_instance->SetData(DATA_PORTALCOUNTER,PortalCounter);
                }
                else
                {
                    if (Creature* tempcreature=m_creature->SummonCreature(C_PORTAL,BossPortal[0],BossPortal[1],BossPortal[2],BossPortal[3],TEMPSUMMON_MANUAL_DESPAWN,0))
                    {
                        Portal[NUMBEROFPORTALS]=tempcreature->GetGUID();
                        PortalSpawnTimer=(uint32)-1;
                        tempcreature->AI()->DoAction(BOSS);
                        isBoss=true;
                    }
                    PortalCounter++;
                    m_instance->SetData(DATA_PORTALCOUNTER,PortalCounter);
                }
            }
        }
        else PortalSpawnTimer-=diff;
    }            
};

#define SPELL_TELEPORT 51347
#define SPELL_DISRUPTION 58291

struct MANGOS_DLL_DECL mob_azuresaboteurAI : public npc_escortAI
{
    mob_azuresaboteurAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
        m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_instance;

    uint16 CastTimer;
    bool WaypointSet;
    bool atCell;
    uint8 Casted;
    uint8 SelectedBoss;

    void Reset()
    {
        CastTimer=0;
        WaypointSet=false;
        Casted=0;
        atCell=false;
        m_instance=NULL;
    }

    void WaypointReached(uint32 point)
    {
        if ((point+1)==NumberOfWaypoints[SelectedBoss])
            atCell=true;
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (!WaypointSet)
        {
            if (!m_instance)
                return;
            if (m_instance->GetData(DATA_PORTALCOUNTER)==6)
                SelectedBoss=m_instance->GetData(DATA_FIRSTBOSS);
            else SelectedBoss=m_instance->GetData(DATA_SECONDBOSS);
            switch (SelectedBoss)
            {
            case 0: //Lavanthor
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,LavanthorReleaseWP[i][0],LavanthorReleaseWP[i][1],LavanthorReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,LavanthorReleaseWP[i][0],LavanthorReleaseWP[i][1],LavanthorReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            case 1: //Moragg
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,MoraggReleaseWP[i][0],MoraggReleaseWP[i][1],MoraggReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,MoraggReleaseWP[i][0],MoraggReleaseWP[i][1],MoraggReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            case 2: //Zuramat
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,ZuramatReleaseWP[i][0],ZuramatReleaseWP[i][1],ZuramatReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,ZuramatReleaseWP[i][0],ZuramatReleaseWP[i][1],ZuramatReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            case 3: //Erekem
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,ErekemReleaseWP[i][0],ErekemReleaseWP[i][1],ErekemReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,ErekemReleaseWP[i][0],ErekemReleaseWP[i][1],ErekemReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            case 4: //Ichoron
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,IchoronReleaseWP[i][0],IchoronReleaseWP[i][1],IchoronReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,IchoronReleaseWP[i][0],IchoronReleaseWP[i][1],IchoronReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            case 5: //Xevozz
                for (uint8 i=0;i<NumberOfWaypoints[SelectedBoss];i++)
                {
                    if (!i)
                        AddWaypoint(i,XevozzReleaseWP[i][0],XevozzReleaseWP[i][1],XevozzReleaseWP[i][2],500);
                    else
                        AddWaypoint(i,XevozzReleaseWP[i][0],XevozzReleaseWP[i][1],XevozzReleaseWP[i][2]);
                }
                SetDespawnAtEnd(false);
                Start(true);
                WaypointSet=true;
                break;
            }
        }

        if (atCell)
        {
            if (CastTimer<=diff)
            {
                if (Casted<3)
                {
                    DoCast(m_creature,SPELL_DISRUPTION,false);
                    CastTimer=1200;
                    Casted++;
                }
                else
                {
                    if (Casted==3)
                    {
                        Casted++;
                        switch (SelectedBoss)
                        {
                        case 0:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature,191566,1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature,SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        case 1:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature,191606,1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature,SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        case 2:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191565, 1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature, SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        case 3:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191564, 1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191563, 1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191562, 1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature, SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        case 4:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191722,1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature,SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        case 5:
                            if (GameObject* pCell = GetClosestGameObjectWithEntry(m_creature, 191556,1000.0f))
                                pCell->SetGoState(GO_STATE_ACTIVE);
                            DoCast(m_creature,SPELL_TELEPORT,false);
                            CastTimer=1500;
                            break;
                        }
                    }
                    else
                    {                        
                        Creature* Boss=NULL;
                        switch (SelectedBoss)
                        {
                        case 0:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(LAVANTHOR_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);                   
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;
                        case 1:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(MORAGG_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;
                        case 2:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(ZURAMAT_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;
                        case 3:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(EREKEM_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;
                        case 4:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(ICHORON_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;                            
                        case 5:
                            Boss=(Creature*)Unit::GetUnit(*m_creature,m_instance->GetData64(XEVOZZ_GUID));
                            if (Boss)
                                Boss->AI()->DoAction(BOSS_PULL);
                            ((TemporarySummon*)m_creature)->UnSummon();
                            break;

                        }
                    }
                }
            }
            else CastTimer-=diff;
        }
    }
};

#define S_ARCANE_BARRAGE_BINDER     58456
#define S_ARCANE_EXPLOSION_BINDER   58455

struct MANGOS_DLL_DECL mob_azurebinderAI : public violet_hold_invaderAI
{
    mob_azurebinderAI(Creature* pCreature) : violet_hold_invaderAI(pCreature)
    {
        Reset();
    }

    uint16 BarrageTimer;
    uint16 ExplosionTimer;

    void Reset()
    {
        violet_hold_invaderAI::Reset();
        BarrageTimer=5000;
        ExplosionTimer=10000;
    }

    void EnterEvadeMode()
    {
        violet_hold_invaderAI::EnterEvadeMode(false);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_invaderAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (BarrageTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_ARCANE_BARRAGE_BINDER);
            BarrageTimer=urand(6000,9000);
        }
        else BarrageTimer-=diff;

        if (ExplosionTimer<diff)
        {
            DoCast(m_creature,S_ARCANE_EXPLOSION_BINDER);
            ExplosionTimer=urand(8000,12000);
        }
        else ExplosionTimer-=diff;       

        DoMeleeAttackIfReady();
    }

};

#define S_IMPALE_INVADER        58459
#define S_CLEAVE_INVADER        15496

struct MANGOS_DLL_DECL mob_azureinvaderAI : public violet_hold_invaderAI
{
    mob_azureinvaderAI(Creature* pCreature) : violet_hold_invaderAI(pCreature)
    {
        Reset();
    }

    uint16 CleaveTimer;
    uint16 ImpaleTimer;

    void Reset()
    {
        violet_hold_invaderAI::Reset();
        CleaveTimer=urand(8000,11000);
        ImpaleTimer=urand(13000,15000);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_invaderAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (ImpaleTimer<diff)
        {
            if (m_creature->GetDistance(m_creature->getVictim())<=5.0f)
            {
                DoCast(m_creature->getVictim(),S_IMPALE_INVADER);
                ImpaleTimer=urand(13000,16000);
            }
        }
        else ImpaleTimer-=diff;

        if (CleaveTimer<diff)
        {
            DoCast(m_creature,S_CLEAVE_INVADER);
            CleaveTimer=urand(5000,9000);
        }
        else CleaveTimer-=diff;

        DoMeleeAttackIfReady();
    }
};

#define S_SLOW_SPELLBREAKER             25603
#define S_ARCANE_BARRAGE_SPELLBREAKER   58462

struct MANGOS_DLL_DECL mob_azurespellbreakerAI : public violet_hold_invaderAI
{
    mob_azurespellbreakerAI(Creature* pCreature) : violet_hold_invaderAI(pCreature)
    {
        Reset();
    }

    uint16 SlowTimer;
    uint16 ArcaneBlastTimer;

    void Reset()
    {
        violet_hold_invaderAI::Reset();
        SlowTimer=urand(3000,6000);
        ArcaneBlastTimer=urand(10000,15000);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_invaderAI::UpdateEscortAI(diff);
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SlowTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_SLOW_SPELLBREAKER);
            SlowTimer=urand(7000,10000);
        }
        else SlowTimer-=diff;

        if (ArcaneBlastTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_ARCANE_BARRAGE_SPELLBREAKER);
            ArcaneBlastTimer=urand(13000,16000);
        }
        else ArcaneBlastTimer-=diff;

        DoMeleeAttackIfReady();
    }
};

#define S_ARCANE_EMPOWERMENT_MAGESLAYER     58469

struct MANGOS_DLL_DECL mob_azuremageslayerAI : public violet_hold_invaderAI
{
    mob_azuremageslayerAI(Creature* pCreature) : violet_hold_invaderAI(pCreature)
    {
        Reset();
    }

    uint16 ArcaneEmpowermentTimer;

    void Reset()
    {
        violet_hold_invaderAI::Reset();
        ArcaneEmpowermentTimer=urand(3000,6000);
    }
    
    void UpdateEscortAI(const uint32 diff)
    {
        violet_hold_invaderAI::UpdateEscortAI(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (ArcaneEmpowermentTimer<diff)
        {
            DoCast(m_creature,S_ARCANE_EMPOWERMENT_MAGESLAYER);
            ArcaneEmpowermentTimer=urand(14000,20000);
        }
        else ArcaneEmpowermentTimer-=diff;

        DoMeleeAttackIfReady();
    }
};


struct MANGOS_DLL_DECL npc_violetholdguardAI : public npc_escortAI
{
    npc_violetholdguardAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        SetWPs();
    }

    void SetWPs()
    {
        WaypointSet=false;
    }

    void Reset()
    {
    }

    bool WaypointSet;

    void DoAction(uint32 action)
    {
        Start(true);
    }

    void WaypointReached(uint32 point)
    {
        if (point)
            ((TemporarySummon*)m_creature)->UnSummon();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!WaypointSet)
        {
            for (uint8 i=0;i<2;i++)
                AddWaypoint(i,finishWP[i][0],finishWP[i][1],finishWP[i][2]);
            WaypointSet=true;
            SetDespawnAtEnd(true);
        }
        npc_escortAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_sinclari(Player *player, Creature *_Creature)
{
    ScriptedInstance* m_instance = (ScriptedInstance*)_Creature->GetInstanceData();
    if (m_instance)
        if (m_instance->GetData(DATA_COMPLETED))
        {
            player->SEND_GOSSIP_MENU(/*?????*/8855, _Creature->GetGUID());
            return true;
        }
    if (!((npc_sinclariAI*)_Creature->AI())->EventActive)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"Activate the crystals when we get in trouble, right.",GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(13853,_Creature->GetGUID());
    }
    else
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"I\'m not fighting, so send me in now!",GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+3);
        player->SEND_GOSSIP_MENU(14271,_Creature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_sinclari(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"Get your people to safety, we\'ll keep the Blue Dragonflight\'s forces at bay.",GOSSIP_ACTION_INFO_DEF+1,GOSSIP_ACTION_INFO_DEF+2);
        player->SEND_GOSSIP_MENU(13854,_Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        ((npc_sinclariAI*)_Creature->AI())->EventActivate();
        player->CLOSE_GOSSIP_MENU();
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->TeleportTo(_Creature->GetMapId(),TeleIn[0],TeleIn[1],TeleIn[2],TeleIn[3]);
        player->CLOSE_GOSSIP_MENU();
        break;
    }
    return true;
}

CreatureAI* GetAI_npc_sinclari(Creature* pCreature)
{
    return new npc_sinclariAI(pCreature);
} 

CreatureAI* GetAI_npc_violetholdguard(Creature* pCreature)
{
    return new npc_violetholdguardAI(pCreature);
} 

CreatureAI* GetAI_mob_portalguardian(Creature* pCreature)
{
    return new mob_portalguardianAI(pCreature);
}  

CreatureAI* GetAI_mob_portalkeeper(Creature* pCreature)
{
    return new mob_portalkeeperAI(pCreature);
}

CreatureAI* GetAI_mob_azurecaptain(Creature* pCreature)
{
    return new mob_azurecaptainAI(pCreature);
}

CreatureAI* GetAI_mob_azureraider(Creature* pCreature)
{
    return new mob_azureraiderAI(pCreature);
}

CreatureAI* GetAI_mob_azuresorceror(Creature* pCreature)
{
    return new mob_azuresorcerorAI(pCreature);
}

CreatureAI* GetAI_mob_azurestalker(Creature* pCreature)
{
    return new mob_azurestalkerAI(pCreature);
}


CreatureAI* GetAI_mob_azurebinder(Creature* pCreature)
{
    return new mob_azurebinderAI(pCreature);
}

CreatureAI* GetAI_mob_azureinvader(Creature* pCreature)
{
    return new mob_azureinvaderAI(pCreature);
}

CreatureAI* GetAI_mob_azurespellbreaker(Creature* pCreature)
{
    return new mob_azurespellbreakerAI(pCreature);
}

CreatureAI* GetAI_mob_azuremageslayer(Creature* pCreature)
{
    return new mob_azuremageslayerAI(pCreature);
}

CreatureAI* GetAI_mob_azuresaboteur(Creature* pCreature)
{
    return new mob_azuresaboteurAI(pCreature);
} 

CreatureAI* GetAI_npc_violetholddoor(Creature* pCreature)
{
    return new npc_violetholddoorAI(pCreature);
}

CreatureAI* GetAI_npc_violetholdportal(Creature* pCreature)
{
    return new npc_violetholdportalAI(pCreature);
}

void AddSC_violet_hold()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_violetholdguard";
    newscript->GetAI = &GetAI_npc_violetholdguard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_violetholddoor";
    newscript->GetAI = &GetAI_npc_violetholddoor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_violetholdportal";
    newscript->GetAI = &GetAI_npc_violetholdportal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_azurebinder";
    newscript->GetAI = &GetAI_mob_azurebinder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_azureinvader";
    newscript->GetAI = &GetAI_mob_azureinvader;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_azurespellbreaker";
    newscript->GetAI = &GetAI_mob_azurespellbreaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_azuremageslayer";
    newscript->GetAI = &GetAI_mob_azuremageslayer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_azuresaboteur";
    newscript->GetAI = &GetAI_mob_azuresaboteur;
    newscript->RegisterSelf();

    newscript=new Script;
    newscript->Name = "mob_portalguardian";
    newscript->GetAI = &GetAI_mob_portalguardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_portalkeeper";
    newscript->GetAI = &GetAI_mob_portalkeeper;
    newscript->RegisterSelf();

    newscript=new Script;
    newscript->Name = "mob_azurecaptain";
    newscript->GetAI = &GetAI_mob_azurecaptain;
    newscript->RegisterSelf();

    newscript=new Script;
    newscript->Name = "mob_azureraider";
    newscript->GetAI = &GetAI_mob_azureraider;
    newscript->RegisterSelf();

    newscript=new Script;
    newscript->Name = "mob_azuresorceror";
    newscript->GetAI = &GetAI_mob_azuresorceror;
    newscript->RegisterSelf();

    newscript=new Script;
    newscript->Name = "mob_azurestalker";
    newscript->GetAI = &GetAI_mob_azurestalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sinclari";
    newscript->GetAI = &GetAI_npc_sinclari;
    newscript->pGossipHello = &GossipHello_npc_sinclari;
    newscript->pGossipSelect = &GossipSelect_npc_sinclari;
    newscript->RegisterSelf();
}