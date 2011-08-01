#include "precompiled.h"
#include "violet_hold_invaderAI.h"
#include "escort_ai.h"

const float portal1WP[7][3]=
{
    {1930.763f, 812.167f, 52.313f},
    {1926.611f, 818.206f, 47.313f},
    {1921.804f, 822.188f, 42.375f},
    {1913.745f, 821.235f, 38.864f},
    {1869.058f, 807.015f, 38.787f},
    {1861.101f, 804.369f, 44.009f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal2WP[5][3]=
{
    {1872.604f, 843.070f, 43.334f},
    {1871.517f, 836.061f, 38.872f},
    {1862.848f, 816.928f, 38.966f},
    {1856.563f, 811.473f, 44.009f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal3WP[5][3]=
{
    {1885.088f, 761.898f, 47.667f},
    {1875.629f, 772.547f, 39.326f},
    {1862.138f, 790.789f, 38.952f},
    {1857.258f, 796.933f, 44.009f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal4WP[3][3]=
{
    {1861.687f, 789.985f, 38.838f},
    {1858.142f, 797.087f, 43.990f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal5WP[3][3]=
{
    {1867.776f, 811.364f, 38.768f},
    {1859.597f, 808.374f, 44.009f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal6WP[3][3]=
{
    {1869.461f, 803.878f, 38.739f},
    {1861.225f, 803.856f, 44.008f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal7WP[8][3]=
{  
    {1928.62f, 837.34f, 47.031f},
    {1929.25f, 831.97f, 45.501f},
    {1926.19f, 826.70f, 43.951f},
    {1912.81f, 821.19f, 38.681f},
    {1887.65f, 808.86f, 38.406f},
    {1869.461f, 803.878f, 38.739f},
    {1861.225f, 803.856f, 44.008f},
    {1858.293f, 804.123f, 44.009f},
};

const float portal8WP[8][3]=
{
    {1921.264f, 760.725f, 50.802f},
    {1910.417f, 755.258f, 47.704f},
    {1900.811f, 751.267f, 47.666f},
    {1885.088f, 761.898f, 47.667f},
    {1875.629f, 772.547f, 39.326f},
    {1862.138f, 790.789f, 38.952f},
    {1857.258f, 796.933f, 44.009f},
    {1858.293f, 804.123f, 44.009f},
};

const float finishWP[2][3]=
{
    {1836.821f, 803.990f, 44.243f},
    {1801.385f, 804.238f, 44.363f},
};

violet_hold_invaderAI::violet_hold_invaderAI(Creature* pCreature) : npc_escortAI(pCreature)
{
    Reset();
    m_instance = (ScriptedInstance*)pCreature->GetInstanceData();
    m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
}

void violet_hold_invaderAI::Reset()
{
    PortalNumber=0;
    EventActive=false;
    WaypointSet=false;
    AttackingDoor=false;
    AttackingDoorTimer=500;
}

void violet_hold_invaderAI::DoAction(uint32 action)
{
    if (action==ATTACKINGDOOR)
    {
        AttackingDoor=true;
        return;
    }
        
    if (action>NUMBEROFPORTALS) 
    {
        EventActive=true;
        PortalNumber=action-NUMBEROFPORTALS;
    }
    else PortalNumber=action;
}

void violet_hold_invaderAI::DamageTaken(Unit* who, uint32 &damage)
{
    if (AttackingDoor)
    {
        AttackingDoor=false;
        m_creature->CastStop();
        m_creature->AI()->AttackStart(who);
    }
}

void violet_hold_invaderAI::UpdateEscortAI(const uint32 diff)
{
    if (!WaypointSet)
    {
        uint8 i=0;
        switch (PortalNumber)
        {
        case 1:
            for (i=0;i<7;i++)
            {
                AddWaypoint(i, portal1WP[i][0], portal1WP[i][1], portal1WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 2:
            for (i=0;i<5;i++)
            {
                AddWaypoint(i, portal2WP[i][0], portal2WP[i][1], portal2WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;   
            break;
        case 3:
            for (i=0;i<5;i++)
            {
                AddWaypoint(i, portal3WP[i][0], portal3WP[i][1], portal3WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 4:
            for (i=0;i<3;i++)
            {
                AddWaypoint(i, portal4WP[i][0], portal4WP[i][1], portal4WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 5:
            for (i=0;i<3;i++)
            {
                AddWaypoint(i, portal5WP[i][0], portal5WP[i][1], portal5WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 6:
            for (i=0;i<3;i++)
            {
                AddWaypoint(i, portal6WP[i][0], portal6WP[i][1], portal6WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 7:
            for (i=0;i<8;i++)
            {
                AddWaypoint(i, portal7WP[i][0], portal7WP[i][1], portal7WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        case 8:
            for (i=0;i<8;i++)
            {
                AddWaypoint(i, portal8WP[i][0], portal8WP[i][1], portal8WP[i][2]);
            }
            SetDespawnAtEnd(true);
            Start(true);
            WaypointSet=true;
            break;
        }
        float modx,mody;
        modx=urand(28,39);
        modx=modx/2;
        mody=sqrt(380-pow(modx,2));
        if (m_instance)
        {
            Creature* tDoor=m_creature->GetMap()->GetCreature(m_instance->GetData64(DOOR_GUID));
            if (rand()%2)                    
                AddWaypoint(i, tDoor->GetPositionX()+modx,tDoor->GetPositionY()+mody,44.019f);
            else
                AddWaypoint(i, tDoor->GetPositionX()+modx,tDoor->GetPositionY()-mody,44.019f);
            AddWaypoint(i+1, finishWP[0][0],finishWP[0][1],finishWP[0][2]);
            AddWaypoint(i+2, finishWP[1][0],finishWP[1][1],finishWP[1][2]);
        }
    }

    if (AttackingDoor)
    {
        if (AttackingDoorTimer<diff)
        {
            if (m_instance)
            {
                m_instance->SetData(DATA_TICKCOUNTER,m_instance->GetData(DATA_TICKCOUNTER)+1);
                AttackingDoorTimer=500;
            }
        }
        else AttackingDoorTimer-=diff;
    }
}

void violet_hold_invaderAI::EnterEvadeMode(bool DoReset = true)
{
    if (DoReset)
        npc_escortAI::EnterEvadeMode();
    else 
    {
        uint8 TPortalNumber=PortalNumber;
        bool TEventActive=EventActive;
        bool TWaypointSet=WaypointSet;
        bool TAttackingDoor=AttackingDoor;
        uint16 TAttackingDoorTimer=AttackingDoorTimer;
        npc_escortAI::EnterEvadeMode();
        PortalNumber=TPortalNumber;
        EventActive=TEventActive;
        WaypointSet=TWaypointSet;
        AttackingDoor=TAttackingDoor;
        AttackingDoorTimer=TAttackingDoorTimer;
    }
}

violet_hold_squadAI::violet_hold_squadAI(Creature* pCreature) : violet_hold_invaderAI(pCreature)
{
    Reset();
}

void violet_hold_squadAI::Reset()
{
    violet_hold_invaderAI::Reset();
    EventActive=true;
    MotherPortal=0;
    ArrayPosition=5;
}

void violet_hold_squadAI::DoAction(uint32 action)
{
    if (action==ATTACKINGDOOR)
    {
        AttackingDoor=true;
        return;
    }
    
    if (action>SQUAD_LIMIT)
        ArrayPosition=action-FIRST_SQUAD;        
    else PortalNumber=action;
}

void violet_hold_squadAI::JustDied(Unit* who)
{
    if (MotherPortal)
    {
        Creature* tempcreature=m_creature->GetMap()->GetCreature(MotherPortal);
        if (tempcreature)
            tempcreature->AI()->DoAction(FIRST_SQUAD+ArrayPosition);
    }
}

