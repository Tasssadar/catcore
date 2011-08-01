#include "violethold.h"
#include "escort_ai.h"

struct MANGOS_DLL_DECL violet_hold_invaderAI : public npc_escortAI
{
    violet_hold_invaderAI(Creature* pCreature);

    void Reset();

    void DoAction(uint32 action);

    void WaypointReached(uint32 id) {};

    void DamageTaken(Unit* who, uint32 &damage);

    void UpdateEscortAI(const uint32 diff);

    void EnterEvadeMode(bool DoReset);

    public:
        ScriptedInstance* m_instance;
    
        uint8 PortalNumber;
        bool EventActive;
        bool WaypointSet;
        bool AttackingDoor;
        uint16 AttackingDoorTimer;
        bool m_bIsRegularMode;
};

struct MANGOS_DLL_DECL violet_hold_squadAI : public violet_hold_invaderAI
{
    violet_hold_squadAI(Creature* pCreature);

    void Reset();

    void DoAction(uint32 action);

    void JustDied(Unit* who);

    public :
        uint64 MotherPortal;
        uint8 ArrayPosition;
};