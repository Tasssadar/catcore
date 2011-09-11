/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Malygos
SD%Complete: 90%
SDComment: verify timers
SDAuthor: Tassadar
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"
#include "WorldPacket.h"
#include "Vehicle.h"
#include "Spell.h"

enum
{
    //First, say what it should do:
    //////////////// PHASE 0 ////////////////
    // Malygost cast SPELL_PORTAL_BEAM on some sort of portal until get aggroed
    // by Focusing iris
    //
    //////////////// PHASE 1 ////////////////
    // Arcane Breath - normal spell
    //
    // Vortex        - ouuu this will be tuff :/ there are spawned some triggers,
    //                 maybe pull between them?
    //
    // Power Spark   - from wowhead: "Spawned throughout the fight and slowly
    //                 shift towards Malygos. Once they reach him, they buff him
    //                 with Power Spark, increasing the damage output by 50% for
    //                 10 seconds, stacking multiplicatively. If killed, they
    //                 instead grant players in proximity the same buff,
    //                 Power Spark, which especially is a great buff for melee
    //                 players close to Malygos."
    //
    //////////////// PHASE 2 //////////////// - 50% of health
    // On beggining of this phase, malygos will take off slowly and let
    // players 10s to DPS him, then he will fly around(?) and cast his deep breath
    // NPCs on flying discs will be spawned, they will fly around and do damage to raid,
    // when is NPC killed, disc does NOT disappear - its vehicle, players use it to!
    // Players also have to cover in protective bubbles(which are spawned continuosly),
    // they shrink over time so players have to run to another one.
    //
    // Surge of Power - Deep Breath....
    // Arcane Storm   - normal spell...
    //
    //////////////// PHASE 3 //////////////// - when all NPCs from previos phase are dead
    // Malygos will destroy platfrom, players are sent to dragon vehicles.
    //
    // Static Field   - trigger with aura
    // Surge of Power - 12s per tick
    //
    //////////////// PHASE 4 //////////////// - When malygos reach 0%
    // Outro, Alexstrasza will arive
    

    // ******************************** SPELLS ******************************** //
    SPELL_BERSERK                  = 64238, // Maybe wrong but enought :)
    //////////////// PHASE 0 ////////////////
    SPELL_PORTAL_BEAM              = 56046,

    //////////////// PHASE 1 ////////////////
    SPELL_ARCANE_BREATH            = 56272,
    SPELL_ARCANE_BREATH_H          = 60072,
    SPELL_VORTEX_DUMMY             = 56105, // This is for addons, actualy does nothing
    SPELL_VORTEX                   = 56266, // Cast on trigger in middle of the room, this interrupt their cast and deal dmg
    SPELL_VORTEX_AOE_VISUAL        = 55873,    // visual effect around platform
    SPELL_POWER_SPARK              = 56152, // If spark reach malygos then buff him, if killed then to players
    SPELL_POWER_SPARK_PLAYERS      = 55852, // This one is propably for players
    SPELL_POWER_SPARK_VISUAL       = 55845,

    //////////////// PHASE 2 ////////////////
    //  SPELL_ARCANE_PULSE             = 57432, // Malygos takes a deep breath... cast on trigger
    SPELL_ARCANE_STORM             = 61693, // AOE
    SPELL_ARCANE_STORM_H           = 61694,
    SPELL_ARCANE_OVERLOAD          = 56432, // Cast this on arcane overload NPCs
    SPELL_ARCANE_BOMB              = 56431, // Cast by arcane overload
    SPELL_DEEP_BREATH              = 60071, // in Deadly boss mod this is as event spell, but not exist in DBC..
    SPELL_SURGE_OF_POWER_BREATH    = 56505, // omfg, they say deep breath, but its this!
    SPELL_DESTROY_PLATFORM_PRE     = 58842, // lights all over the platform
    SPELL_DESTROY_PLATFROM_BOOM    = 59084, // Big Blue boom
    //NPCs spells
    SPELL_ARCANE_SHOCK             = 57058,
    SPELL_ARCANE_SHOCK_H           = 60073,
    //SPELL_ARCANE_BARRAGE           = 63934, //This one has strange data in DBC
    SPELL_ARCANE_BARRAGE           = 58456, // I have to modify basepoints in this spell...
    BP_BARRAGE0                    = 14138,
    BP_BARRAGE0_H                  = 16965,
    SPELL_TELEPORT_VISUAL          = 64446,

    //////////////// PHASE 3 ////////////////
    SPELL_STATIC_FIELD             = 57428, // Summon trigger and cast this on them should be enought
    SPELL_SURGE_OF_POWER           = 57407, // this is on one target
    SPELL_SURGE_OF_POWER_H         = 60936, // this is on unlimited tagets, must limit it in mangos
    
    SPELL_ARCANE_PULSE             = 57432,
    //Dragons spells
    SPELL_FLAME_SPIKE              = 56091,
    SPELL_ENGULF_IN_FLAMES         = 56092,
    SPELL_REVIVIFY                 = 57090,
    SPELL_LIFE_BURST               = 57143,
    SPELL_FLAME_SHIELD             = 57108,
    SPELL_BLAZING_SPEED            = 57092,

    // ******************************** NPCs & GObjects ******************************** //
    ITEM_KEY_TO_FOCUSING_IRIS      = 44582,
    ITEM_KEY_TO_FOCUSING_IRIS_H    = 44581,
    GO_FOCUSING_IRIS               = 193958,
    GO_FOCUSING_IRIS_H             = 193960,
    GO_EXIT_PORTAL                 = 193908,
    //////////////// PHASE 1 ////////////////
    NPC_AOE_TRIGGER                = 22517,
    NPC_VORTEX                     = 30090,
    NPC_POWER_SPARK                = 30084,
    NPC_SPARK_PORTAL               = 30118, // For power sparks
    VEHICLE_VORTEX                 = 168,

    //////////////// PHASE 2 ////////////////
    NPC_HOVER_DISC                 = 30248, // Maybe wrong, two following NPC flying on them (vehicle)
    DISPLAY_HOVER_DISC             = 26876, // DisplayID of hover disc
    NPC_NEXUS_LORD                 = 30245, // two (?) of them are spawned on beginning of phase 2
    NPC_NEXUS_LORD_H               = 31750,
    NPC_SCION_OF_ETERNITY          = 30249, // same, but unknow count
    NPC_SCION_OF_ETERNITY_H        = 31751,
    NPC_ARCANE_OVERLOAD            = 30282, // Bubles
    GO_PLATFORM                    = 193070,

    //////////////// PHASE 3 ////////////////
    NPC_SURGE_OF_POWER             = 30334, // Because its on three targets, malygos cant cast it by hymself
    NPC_STATIC_FIELD               = 30592, // Trigger for that spell. Hope its fly

    //////////////// PHASE 4 ////////////////
    NPC_ALEXSTRASZA                = 32295, // The Life-Binder

    SAY_INTRO1                     = -1616000,
    SAY_INTRO2                     = -1616001,
    SAY_INTRO3                     = -1616002,
    SAY_INTRO4                     = -1616003,
    SAY_INTRO5                     = -1616004,
    SAY_INTRO_PHASE3               = -1616018,
    SAY_AGGRO1                     = -1616005,
    SAY_AGGRO2                     = -1616013,
    SAY_AGGRO3                     = -1616019,
    SAY_VORTEX                     = -1616006,
    SAY_POWER_SPARK                = -1616035,
    SAY_POWER_SPARK_BUFF           = -1616007,
    SAY_KILL1_1                    = -1616008,
    SAY_KILL1_2                    = -1616009,
    SAY_KILL1_3                    = -1616010,
    SAY_KILL2_1                    = -1616020,
    SAY_KILL2_2                    = -1616021,
    SAY_KILL2_3                    = -1616022,
    SAY_KILL3_1                    = -1616023,
    SAY_KILL3_2                    = -1616024,
    SAY_KILL3_3                    = -1616025,
    SAY_END_PHASE1                 = -1616012,
    SAY_END_PHASE2                 = -1616017,
    SAY_ARCANE_PULSE               = -1616014,
    SAY_ARCANE_PULSE_WARN          = -1616015,
    SAY_ARCANE_OVERLOAD            = -1616016,
    SAY_SURGE_OF_POWER             = -1616026,
    SAY_CAST_SPELL1                = -1616027,
    SAY_CAST_SPELL2                = -1616028,
    SAY_CAST_SPELL3                = -1616029,
    SAY_OUTRO1                     = -1616030,
    SAY_OUTRO2                     = -1616031,
    SAY_OUTRO3                     = -1616032,
    SAY_OUTRO4                     = -1616033,
    SAY_OUTRO5                     = -1616034,

    SHELL_MIN_X                    = 722,
    SHELL_MAX_X                    = 768,
    SHELL_MIN_Y                    = 1290,
    SHELL_MAX_Y                    = 1339,

    MOVE_ANIM_TAKEOFF              = 3,    // For use in SMSG_MONSTER_MOVE
    MOVE_ANIM_LAND                 = 0,

    NEXUS_LORD_COUNT               = 2,
    NEXUS_LORD_COUNT_H             = 4,
    SCION_OF_ETERNITY_COUNT        = 4,
    SCION_OF_ETERNITY_COUNT_H      = 8,

    PHASE_NOSTART                  = 0,
        SUBPHASE_FLY_DOWN          = 04,
    PHASE_FLOOR                    = 1,
        SUBPHASE_VORTEX            = 11,
    PHASE_ADDS                     = 2,
        SUBPHASE_TALK              = 21,
    PHASE_DRAGONS                  = 3,
        SUBPHASE_DESTROY_PLATFORM1 = 31,
        SUBPHASE_DESTROY_PLATFORM2 = 32,
        SUBPHASE_DESTROY_PLATFORM3 = 33,
    PHASE_OUTRO                    = 4,
        SUBPHASE_STOP_COMBAT       = 41,
        SUBPHASE_DIE               = 42,
};

static const float GOPositions[][4]=
{
    {754.346f, 1300.87f, 256.249f, 3.14159f},   // Raid Platform position
    {754.731f, 1300.12f, 266.171f, 5.01343f},   // Focusing iris and Alexstrazas gift
    {724.684f, 1332.92f, 267.234f, -0.802851f}, // Exit Portal
};
static const float SparkLoc[][2]=
{
    {652.417f, 1200.52f},
    {847.67f, 1408.05f},
    {647.675f, 1403.8f},
    {843.182f, 1215.42f},
};

static const float OtherLoc[][4]=
{
    {808.0f, 1301.0f, 268.0f, 0},          // 0 Phase 3 position 
    {789.0f, 1302.0f, 288.0f, 2.86f},      // 1 Vortex FarSight loc
    {754.29f, 1301.18f, 266.17f, 0},       // 2 Center of the platform, ground.
    {823.0f, 1241.0f, 299.0f, 0},          // 3 Alexstrasza's  position
    {787.0f, 1152.0f, 299.0f, 0},          // 4 Alexstrasza spawn position
    {778.0f, 1280.12, 299.0f, 0},          // 5 Alexstrasza final position
};

#define MAX_VORTEX              21
#define VORTEX_Z                285.0f

#define FLOOR_Z                 268.17f
#define LANDING_Z               266.17f
#define AIR_Z                   297.24f

enum MovementStates
{
    MOVESTATE_SETTED            = 0x01,
    MOVESTATE_LAND              = 0x02,
    MOVESTATE_COMPLETED         = 0x04,
};

static float vortexAddAngle[2] = { M_PI_F, M_PI_F/2};
static uint8 vortexCount[2] = { 2, 5 };

struct Vortex
{
    Vortex(Creature *me, float angle)
    {
        m_creature = me;
        m_angle = angle;
        m_fRotateAngle = 0;
        first = true;
    }

    void add(Player *plr)
    {
       plrList.push_back(plr);
    }

    void Teleport(bool center)
    {
        float x = OtherLoc[2][0];
        float y = OtherLoc[2][1];
        if(!center)
        {
            x += cos(m_angle)*25;
            y += sin(m_angle)*25;
        }
        Player *plr = NULL;
        for(uint8 i = 0; i < plrList.size(); ++i)
        {
            plr = plrList[i];
            if(!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;
            if(center)
                plr->GetCamera().SetView(plr);
            plr->NearTeleportTo(x, y, VORTEX_Z, 0);
        }
    }

    void Send()
    {
        if(m_fRotateAngle == 0)
        {
            float m_fLenght = (2*M_PI_F*25)/8;
            m_fRotateAngle = (2*M_PI_F) / m_fLenght;
        }

        uint16 max = ceil((2*M_PI_F)/m_fRotateAngle);
        PointPath vorPath;
        vorPath.resize(max+1);

        float x,y;
        for(uint16 i = 1; i <= max; ++i)
        {
            m_angle += m_fRotateAngle;
            m_angle -= m_angle > 2*M_PI_F ? 2*M_PI_F : 0;
            x = OtherLoc[2][0] + cos(m_angle)*25;
            y = OtherLoc[2][1] + sin(m_angle)*25;
            vorPath.set(i, Coords(x,y, VORTEX_Z));
        }

        Creature *pVortex = NULL;
        if(first)
            pVortex = m_creature->SummonCreature(NPC_VORTEX, OtherLoc[1][0], OtherLoc[1][1], OtherLoc[1][2], OtherLoc[1][3], TEMPSUMMON_TIMED_DESPAWN, 18000);

        Player *plr = NULL;
        for(uint8 i = 0; i < plrList.size(); ++i)
        {
            plr = plrList[i];
            if(!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;
            if(pVortex)
            {
                plr->CastSpell(plr, SPELL_VORTEX, true, NULL, NULL, m_creature->GetGUID());
                plr->GetCamera().SetView(pVortex);
            }
            vorPath.set(0,plr->GetPosition());
            plr->ChargeMonsterMove(vorPath, SPLINETYPE_NORMAL, SPLINEFLAG_TRAJECTORY, 2600);
        }
        first = false;
    }

    bool first;
    float m_angle;
    float m_fRotateAngle;
    std::vector<Player*> plrList;
    Creature *m_creature;
};

/*######
## boss_malygos
######*/

struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
        pAlexstrasza = NULL;
        pTrigger = NULL;
        pFloor = NULL;
        pPortalDummy = NULL;
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsFlying;
    float moveX, moveY, moveZ;
    uint8 m_bLand;
    bool m_bBeamzPortal;

    Creature *pAlexstrasza;
    Creature *pTrigger;
    GameObject *pFloor;
    Creature *pPortalDummy;
    
    uint8 m_uiPhase; //Fight Phase
    uint8 m_uiSubPhase; //Subphase if needed
    uint8 m_uiSpeechCount;
    uint8 m_uiVortexPhase;
    std::vector<Creature*> m_lSparkGUIDList;
    GuidList m_lDiscGUIDList;

    uint32 m_uiEnrageTimer;
    uint32 m_uiSpeechTimer[5];
    uint32 m_uiTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiArcaneBreathTimer;
    uint32 m_uiPowerSparkTimer;
    uint32 m_uiDeepBreathTimer;
    uint32 m_uiShellTimer;
    uint32 m_uiArcaneStormTimer;
    uint32 m_uiStaticFieldTimer;
    uint32 m_uiArcanePulseTimer;
    uint32 m_uiSurgeOfPowerTimer;
    uint32 m_uiMovementTimer;

    uint8 m_moveState;

    std::vector<Vortex*> m_vortexList;

    void Reset()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MALYGOS, NOT_STARTED);
            m_pInstance->SetData(TYPE_LIGHT, LIGHT_NORMAL);
        }
        else
            m_creature->ForcedDespawn();
        m_bLand = 0;
        m_uiPhase = PHASE_NOSTART;
        m_uiSubPhase = 0;
        m_uiSpeechCount = 0;
        m_uiVortexPhase = 0;
        m_lSparkGUIDList.clear();
        m_uiMovementTimer = 0;
        m_bBeamzPortal = false;

        m_uiEnrageTimer = 600000;
        m_uiSpeechTimer[0] = 15000;
        m_uiSpeechTimer[1] = 22000;
        m_uiSpeechTimer[2] = 24000;
        m_uiSpeechTimer[3] = 26000;
        m_uiSpeechTimer[4] = 23000;
        m_uiSpeechTimer[5] = 22000;
        m_uiTimer = 7000;
        m_uiVortexTimer = 60000;
        m_uiArcaneBreathTimer = 15000;
        m_uiPowerSparkTimer = 30000;
        m_uiDeepBreathTimer = 70000;
        m_uiShellTimer = 0;
        m_uiArcaneStormTimer = 15000;
        m_uiStaticFieldTimer = 15000;
        m_uiArcanePulseTimer = 1000;
        m_uiSurgeOfPowerTimer = 30000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 3.5f, true);
        m_creature->SetSpeedRate(MOVE_RUN, 3.5f, true);
        m_creature->SetSpeedRate(MOVE_WALK, 3.5f, true);

        m_moveState = 0;
        DeleteVortex();
    }

    void JustReachedHome()
    {
        Reset();
        //Summon Platform
        if (!GetClosestGameObjectWithEntry(m_creature, GO_PLATFORM, 120.0f))
            pFloor = m_creature->SummonGameobject(GO_PLATFORM, GOPositions[0][0], GOPositions[0][1], GOPositions[0][2], GOPositions[0][3], 0);

        //Summon focusing iris
        if (!GetClosestGameObjectWithEntry(m_creature, m_bIsRegularMode ? GO_FOCUSING_IRIS : GO_FOCUSING_IRIS_H, 120.0f))
            m_creature->SummonGameobject(m_bIsRegularMode ? GO_FOCUSING_IRIS : GO_FOCUSING_IRIS_H, GOPositions[1][0], GOPositions[1][1], GOPositions[1][2], GOPositions[1][3], 0);

        //Summon exit portal
        if (!GetClosestGameObjectWithEntry(m_creature, GO_EXIT_PORTAL, 120.0f))
            m_creature->SummonGameobject(GO_EXIT_PORTAL, GOPositions[2][0], GOPositions[2][1], GOPositions[2][2], GOPositions[2][3], 0);

        //Despawn all summoned creatures	 
        DespawnCreatures(NPC_POWER_SPARK, 150.0f);
        DespawnCreatures(NPC_ARCANE_OVERLOAD, 150.0f);
        DespawnCreatures(NPC_NEXUS_LORD, 150.0f);
        DespawnCreatures(NPC_SCION_OF_ETERNITY, 150.0f);
        DespawnCreatures(NPC_HOVER_DISC, 70.0f, true);
        DespawnCreatures(NPC_STATIC_FIELD, 150.0f);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiPhase != PHASE_FLOOR && m_uiPhase != PHASE_DRAGONS && !m_creature->HasAura(SPELL_BERSERK))
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            if (m_uiPhase == PHASE_FLOOR || m_creature->HasAura(SPELL_BERSERK))
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetSpeedRate(MOVE_FLIGHT, 1.0f, true);
        m_creature->SetSpeedRate(MOVE_RUN, 1.0f, true);
        m_creature->SetSpeedRate(MOVE_WALK, 1.0f, true);
        m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
        DoScriptText(SAY_AGGRO1, m_creature);
        //Despawn exit portal
        if (GameObject *pPortal = GetClosestGameObjectWithEntry(m_creature, GO_EXIT_PORTAL, 120.0f))
            pPortal->Delete();

        pFloor = GetClosestGameObjectWithEntry(m_creature, GO_PLATFORM, 180.0f);
        pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_AOE_TRIGGER, 180.0f);

        if (m_pInstance->GetData(TYPE_OUTRO_CHECK) == 1) //Should be enought to trigger outro immediatly
        {
            //Destroy Platform
            if (pTrigger)
                pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFROM_BOOM, false);
            if (pFloor)
                pFloor->Delete();

            //Mount Players
            MountPlayers();

            m_uiPhase = PHASE_OUTRO;
            m_uiSubPhase = SUBPHASE_STOP_COMBAT;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (m_uiPhase == PHASE_OUTRO && m_uiSubPhase != SUBPHASE_DIE)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage >= m_creature->GetHealth() && m_uiSubPhase != SUBPHASE_DIE)
        {
            m_uiPhase = PHASE_OUTRO;
            m_uiSubPhase = SUBPHASE_STOP_COMBAT;
            uiDamage = 0;
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_pInstance->SetData(TYPE_MALYGOS, DONE);
        m_pInstance->SetData(TYPE_OUTRO_CHECK, 0);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        uint8 text = 0;
        switch(m_uiPhase)
        {
            case PHASE_FLOOR:   text = urand(0, 2); break;
            case PHASE_ADDS:    text = urand(3, 5); break;
            case PHASE_DRAGONS: text = urand(6, 8); break;
            default: return;
        }
        DoScriptText(SAY_KILL1_1-text, m_creature);
    }

    void SummonedCreatureJustDied(Creature* pDespawned)
    {
        if ((pDespawned->GetDisplayId() != 24316 && pDespawned->GetDisplayId() != 24317 &&
            pDespawned->GetDisplayId() != 24318 && pDespawned->GetDisplayId() != 24319)
            || m_uiPhase == PHASE_NOSTART)
            return;

        if(pDespawned->GetVehicleGUID())
        {
            Vehicle *pDisc = m_creature->GetMap()->GetVehicle(pDespawned->GetVehicleGUID());
            ((Creature*)pDisc)->SetSpeedRate(MOVE_FLIGHT, 3.5f, true);
            ((Creature*)pDisc)->SetSpeedRate(MOVE_RUN, 3.5f, true);
            ((Creature*)pDisc)->SetSpeedRate(MOVE_WALK, 3.5f, true);
            ((Creature*)pDisc)->setFaction(35);

            float x,y,z;
            pDespawned->GetPosition(x,y,z);
            pDespawned->UpdateGroundPositionZ(x, y, z, 100);
            ((Creature*)pDisc)->GetMotionMaster()->Clear(false, true);
            ((Creature*)pDisc)->GetMotionMaster()->MovePoint(0, x, y, z);

            m_lDiscGUIDList.push_back(((Creature*)pDisc)->GetGUID());
        }
    }

    void DoMovement(float x, float y, float z)
    {
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MovePoint(1, x,y,z);
        m_moveState &= ~(MOVESTATE_COMPLETED);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if(uiMoveType != POINT_MOTION_TYPE || uiPointId != 1)
            return;
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_moveState |= MOVESTATE_COMPLETED;
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case 0:
                DoScriptText(SAY_POWER_SPARK_BUFF, m_creature);
            case 1:
                if (m_lSparkGUIDList.empty())
                    break;
                for(std::vector<Creature*>::iterator itr = m_lSparkGUIDList.begin(); !m_lSparkGUIDList.empty() && itr != m_lSparkGUIDList.end();)
                {
                    if (!(*itr) || !(*itr)->IsInWorld() || !(*itr)->isAlive() ||
                        (*itr)->GetVisibility() == VISIBILITY_OFF || (*itr)->GetHealth() == 1)
                    {
                        m_lSparkGUIDList.erase(itr);
                        itr = m_lSparkGUIDList.begin();
                    }else ++itr;
                }
                break;
            case 2:
                m_creature->InterruptNonMeleeSpells(false);
                PortalBeam(true);
                m_creature->RemoveAurasDueToSpell(SPELL_PORTAL_BEAM);

                DoMovement(OtherLoc[2][0], OtherLoc[2][1], m_creature->GetPositionZ());
                moveX = OtherLoc[2][0];
                moveY = OtherLoc[2][1];
                moveZ = LANDING_Z;
                m_moveState |= MOVESTATE_LAND | MOVESTATE_SETTED;
                m_uiSubPhase = SUBPHASE_FLY_DOWN;
                break;
        }
    }

    void TakeOffOn(bool takeoff, float z)
    {
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        float cx, cy, cz;
        m_creature->GetPosition(cx, cy, cz);

        WorldPacket data(SMSG_MONSTER_MOVE);
        data << m_creature->GetPackGUID();
        data << uint8(0);
        data << cx << cy << z;
        data << uint32(getMSTime());
        data << uint8(SPLINETYPE_NORMAL);
        data << uint32(SPLINEFLAG_UNKNOWN3 | SPLINEFLAG_FLYING);
        data << uint8(takeoff ? MOVE_ANIM_TAKEOFF : MOVE_ANIM_LAND);
        data << uint32(0);
        data << uint32(1000);
        data << uint32(1);
        data << cx << cy << z;
        m_creature->SendMessageToSet(&data, false);
        m_creature->GetMap()->CreatureRelocation(m_creature, cx, cy, z, m_creature->GetOrientation());

        if (takeoff)
            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        else
            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);

        m_uiMovementTimer = 2000;
        m_moveState &= ~(MOVESTATE_COMPLETED);
        m_moveState &= ~(MOVESTATE_LAND);
    }

    void DoVortex(uint8 phase)
    {
        switch(phase)
        {
            case 0:
            {
                SetCombatMovement(false);
                TakeOffOn(true, OtherLoc[2][2]+20);
                moveX = OtherLoc[2][0];
                moveY = OtherLoc[2][1];
                moveZ = OtherLoc[2][2]+20;
                m_uiMovementTimer = 1000;
                m_moveState &= ~(MOVESTATE_COMPLETED);
                m_moveState |= MOVESTATE_SETTED;
                m_creature->SetTargetGUID(0);
                break;
            }
            case 1:
            {
                m_creature->SetTargetGUID(0);
                if (pTrigger)
                    pTrigger->CastSpell(pTrigger, SPELL_VORTEX_AOE_VISUAL, false);

                std::vector<Player*> tmpPlr;
                Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    tmpPlr.push_back(itr->getSource());

                float angle = 0;
                for(uint8 i = 0; i < vortexCount[uint8(!m_bIsRegularMode)]; ++i)
                {
                    Vortex *vor = new Vortex(m_creature, angle);
                    for(uint8 y = 5*i; y < 5*(i+1) && y < tmpPlr.size(); ++y)
                        vor->add(tmpPlr[y]);

                    vor->Teleport(false);
                    vor->Send();

                    m_vortexList.push_back(vor);
                    angle += vortexAddAngle[uint8(!m_bIsRegularMode)];
                }
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                for(uint8 i = 0; i < m_vortexList.size(); ++i)
                    m_vortexList[i]->Send();
                break;
            }
            case 5:
            {
                for(uint8 i = 0; i < m_vortexList.size(); ++i)
                    m_vortexList[i]->Teleport(true);

                DeleteVortex();

                SetCombatMovement(true);
                TakeOffOn(false, FLOOR_Z);
                PowerSpark(2);
                m_uiArcaneBreathTimer = 15000 + urand(3000, 8000);
                break;
            }
            case 6:
                if (m_creature->getVictim())
                {
                    m_creature->SetTargetGUID(m_creature->getVictim()->GetGUID());
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }
                m_uiSubPhase = 0;
                break;
        }
    }

    void DeleteVortex()
    {
        for(uint8 i = 0; i < m_vortexList.size(); ++i)
            delete m_vortexList[i];
        m_vortexList.clear();
    }

    void PowerSpark(uint8 action)
    {
        switch(action)
        {
            case 1:
            {
                uint8 random = urand(0, 3);
                if (Creature *pSpark = m_creature->SummonCreature(NPC_POWER_SPARK, SparkLoc[random][0], SparkLoc[random][1], FLOOR_Z+10, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                {
                    pSpark->CastSpell(pSpark, SPELL_POWER_SPARK_VISUAL, false);
                    pSpark->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                    m_lSparkGUIDList.push_back(pSpark);
                }
                break;
            }
            case 2:
            case 3:
            {
                if (m_lSparkGUIDList.empty())
                    break;
                for(std::vector<Creature*>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
                {
                    if (!(*itr) || !(*itr)->IsInWorld() || !(*itr)->isAlive())
                        continue;

                    if(action == 2)
                        (*itr)->GetMotionMaster()->MoveFollow(m_creature, 0.0f, 0.0f);
                    else
                    {
                        (*itr)->GetMotionMaster()->Clear(false, true);
                        (*itr)->GetMotionMaster()->MoveIdle();
                    }
                }
                break;
            }
        }
    }

    void DoSpawnAdds()
    {
        //Nexus lords
        uint8 max_lords = m_bIsRegularMode ? NEXUS_LORD_COUNT : NEXUS_LORD_COUNT_H;
        for(uint8 i=0; i < max_lords;++i)
        {
            uint32 x = urand(SHELL_MIN_X, SHELL_MAX_X);
            uint32 y = urand(SHELL_MIN_Y, SHELL_MAX_Y);
            if (Creature *pLord = m_creature->SummonCreature(NPC_NEXUS_LORD, x, y, FLOOR_Z+10, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                pLord->CastSpell(pLord, SPELL_TELEPORT_VISUAL, true);
                pLord->AI()->AttackStart(m_creature->getVictim());
            }
        }

        //Scions of eternity
        uint8 max_scions = m_bIsRegularMode ? SCION_OF_ETERNITY_COUNT : SCION_OF_ETERNITY_COUNT_H;
        for(uint8 i=0; i < max_scions;++i)
        {
            uint32 x = urand(SHELL_MIN_X, SHELL_MAX_X);
            uint32 y = urand(SHELL_MIN_Y, SHELL_MAX_Y);
            if (Creature *pScion = m_creature->SummonCreature(NPC_SCION_OF_ETERNITY, x, y, FLOOR_Z+10, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                pScion->CastSpell(pScion, SPELL_TELEPORT_VISUAL, true);
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pScion->AI()->AttackStart(pTarget);
            }
        }
    }

    bool IsThereAnyAdd()
    {
        //Search for Nexus lords
        if (GetClosestCreatureWithEntry(m_creature, NPC_NEXUS_LORD, 180.0f))
            return true;

        //Search for Scions of eternity
        if (GetClosestCreatureWithEntry(m_creature, NPC_SCION_OF_ETERNITY, 180.0f))
            return true;

        return false;
    }

    void DoSpawnShell()
    {
        uint32 x = urand(SHELL_MIN_X, SHELL_MAX_X);
        uint32 y = urand(SHELL_MIN_Y, SHELL_MAX_Y);
        if (Creature *pShell = m_creature->SummonCreature(NPC_ARCANE_OVERLOAD, x, y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 45000))
        {
            pShell->CastSpell(pShell, SPELL_ARCANE_OVERLOAD, true);
            pShell->CastSpell(pShell, SPELL_ARCANE_BOMB, false);
        }
    }

    void MountPlayers()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if (lPlayers.isEmpty())
            return;

        Player* pPlayer = NULL;
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            pPlayer = itr->getSource();
            if (!pPlayer || !pPlayer->IsInWorld() || !pPlayer->isAlive())
                continue;

            if (pPlayer->GetVehicleGUID())
                pPlayer->ExitVehicle();

            if (Vehicle *pTemp = m_creature->SummonVehicle(NPC_WYRMREST_SKYTALON, pPlayer->GetPositionX(), pPlayer->GetPositionY(), OtherLoc[0][2], 0))
            {
                m_creature->AddThreat((Creature*)pTemp, 1.0f);  // To not leave combat

                pPlayer->EnterVehicle(pTemp, 0, false);

                uint32 health = ((Creature*)pTemp)->GetHealth() + (pPlayer->GetMaxHealth()*2); // may be wrong
                ((Creature*)pTemp)->SetCreatorGUID(pPlayer->GetGUID());
                ((Creature*)pTemp)->SetMaxHealth(health);
                ((Creature*)pTemp)->SetHealth(health);
                ((Creature*)pTemp)->SetFacingTo(((Creature*)pTemp)->GetAngle(OtherLoc[0][0], OtherLoc[0][1]));
            }
        }
    }

    void KnockUpPlayers()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        if (lPlayers.isEmpty())
            return;

        Player* pPlayer = NULL;
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            pPlayer = itr->getSource();
            if (!pPlayer || !pPlayer->IsInWorld())// || !pPlayer->isAlive()) -- we wanna get dead bodies out
                continue;
            pPlayer->KnockBackFrom(pPlayer, 0, 40);
        }
    }

    void DespawnCreatures(uint32 entry, float distance, bool discs = false)
    {
        //Because vehicles cant be found by GetCreatureListWithEntryInGrid()
        if (discs)
        {
            if (m_lDiscGUIDList.empty())
                return;
            Map *pMap = m_creature->GetMap();

            if (!pMap)
                return;

            for(GuidList::iterator itr = m_lDiscGUIDList.begin(); itr != m_lDiscGUIDList.end(); ++itr)
                if (Vehicle *pVehicle = pMap->GetVehicle(*itr))
                    pVehicle->Dismiss();

            m_lDiscGUIDList.clear();
            return;
        }

        CreatureList m_pCreatures;
        GetCreatureListWithEntryInGrid(m_pCreatures, m_creature, entry, distance);

        if (m_pCreatures.empty())
            return;

        for(CreatureList::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
        {
            (*iter)->DealDamage(*iter, (*iter)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            (*iter)->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiMovementTimer)
        {
            if(m_uiMovementTimer <= uiDiff)
            {
                m_moveState |= MOVESTATE_COMPLETED;
                m_uiMovementTimer = 0;
            }else m_uiMovementTimer -= uiDiff;
        }

        if ((m_moveState & MOVESTATE_SETTED) && (m_moveState & MOVESTATE_COMPLETED))
        {
            if (m_moveState & MOVESTATE_LAND)
                TakeOffOn(false, moveZ);
            else
                DoMovement(moveX, moveY, moveZ);
            m_moveState &= ~(MOVESTATE_SETTED);
        }

        if (m_uiPhase == PHASE_NOSTART)
        {
            if(m_uiSubPhase == SUBPHASE_FLY_DOWN)
            {
                if((m_moveState & MOVESTATE_SETTED) || !(m_moveState & MOVESTATE_COMPLETED) || (m_moveState & MOVESTATE_LAND))
                    return;
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_uiSubPhase = 0;
                m_uiPhase = PHASE_FLOOR;
            }
            else
            {
                //Speech
                if (m_uiSpeechTimer[m_uiSpeechCount] <= uiDiff)
                {
                    DoScriptText(SAY_INTRO1-m_uiSpeechCount, m_creature);
                    m_uiSpeechCount++;
                    if (m_uiSpeechCount == 5)
                    {
                        m_uiSpeechCount = 0;
                        m_uiSpeechTimer[0] = 28000;
                        m_uiSpeechTimer[1] = 22000;
                        m_uiSpeechTimer[2] = 24000;
                        m_uiSpeechTimer[3] = 26000;
                        m_uiSpeechTimer[4] = 23000;
                        m_uiSpeechTimer[5] = 22000;
                    }
                }else m_uiSpeechTimer[m_uiSpeechCount] -= uiDiff;

                //Random movement over platform
                if((m_uiTimer && m_uiTimer <= uiDiff) || (!m_uiTimer && (m_moveState & MOVESTATE_COMPLETED)))
                {
                    if(!m_bBeamzPortal)
                    {
                        PortalBeam(true);
                        uint8 tmp = urand(0,3);
                        float dx = OtherLoc[2][0] - SparkLoc[tmp][0];
                        float dy = OtherLoc[2][1] - SparkLoc[tmp][1];
                        float ang = atan2(dy, dx) + (urand(0,1) == 1 ? (M_PI_F/2) : -(M_PI_F/2));
                        ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
                        float tmpDst = float(urand(35,50));
                        float sparkX = SparkLoc[tmp][0] + (cos(ang)*tmpDst);
                        float sparkY = SparkLoc[tmp][1] + (sin(ang)*tmpDst);

                        DoMovement(sparkX, sparkY, m_creature->GetPositionZ());
                        m_uiTimer = 0;
                        m_bBeamzPortal = true;
                    }
                    else
                    {
                        PortalBeam(false);
                        m_uiTimer = 10000;
                        m_bBeamzPortal = false;
                    }
                }else if(m_uiTimer) m_uiTimer -= uiDiff;
            }
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Enrage timer.....
        if (m_uiEnrageTimer <= uiDiff && m_uiPhase != PHASE_OUTRO)
        {
            SetCombatMovement(true);
            DoCast(m_creature, SPELL_BERSERK, true);
            m_uiEnrageTimer = 600000;
            m_creature->SetSpeedRate(MOVE_FLIGHT, 3.5f, true);
            m_creature->SetSpeedRate(MOVE_RUN, 3.5f, true);
            m_creature->SetSpeedRate(MOVE_WALK, 3.5f, true);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }else m_uiEnrageTimer -= uiDiff;

        switch(m_uiPhase)
        {
            case PHASE_FLOOR:
            {
                switch(m_uiSubPhase)
                {
                    case SUBPHASE_VORTEX:
                    {
                        if (m_uiTimer <= uiDiff)
                        {
                            m_uiTimer = 2500;
                            switch(m_uiVortexPhase)
                            {
                                case 1: DoCast(m_creature, SPELL_VORTEX_DUMMY); break;
                                case 5: m_uiTimer = 1500; break;
                                case 6: m_uiSubPhase = 0; break;
                            }
                            DoVortex(m_uiVortexPhase);

                            ++m_uiVortexPhase;
                        }else m_uiTimer -= uiDiff;
                        return;
                    }
                    default: break;
                }

                //Vortex
                if (m_uiVortexTimer <= uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    PowerSpark(3);
                    DoVortex(0);
                    m_uiVortexPhase = 1;
                    m_uiSubPhase = SUBPHASE_VORTEX;
                    m_uiVortexTimer = 56000;
                    m_uiTimer = 6000;
                    DoScriptText(SAY_VORTEX, m_creature);
                    m_uiArcaneBreathTimer = 15000 + urand(3000, 8000);
                    return;
                }else m_uiVortexTimer -= uiDiff;

                //Arcane Breath
                if (m_uiArcaneBreathTimer <= uiDiff)
                {
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H);
                    m_uiArcaneBreathTimer = 8000 + urand(5000, 13000);
                }else m_uiArcaneBreathTimer -= uiDiff;

                //PowerSpark
                if (m_uiPowerSparkTimer<= uiDiff)
                {
                    PowerSpark(1);
                    DoScriptText(SAY_POWER_SPARK, m_creature);
                    m_uiPowerSparkTimer = 30000;
                }else m_uiPowerSparkTimer -= uiDiff;

                //Health check
                if (m_uiTimer<= uiDiff)
                {
                    uint8 health = m_creature->GetHealth()*100 / m_creature->GetMaxHealth();
                    if (health <= 50)
                    {
                        m_creature->InterruptNonMeleeSpells(true);
                        SetCombatMovement(false);
                        DoScriptText(SAY_END_PHASE1, m_creature);

                        TakeOffOn(true, OtherLoc[2][2]+40);
                        moveX = OtherLoc[2][0];
                        moveY = OtherLoc[2][1];
                        moveZ = OtherLoc[2][2]+40;
                        m_uiMovementTimer = 2000;
                        m_moveState &= ~(MOVESTATE_COMPLETED);
                        m_moveState |= MOVESTATE_SETTED;

                        //Despawn power sparks
                        DespawnCreatures(NPC_POWER_SPARK, 180.0f);
                        m_uiPhase = PHASE_ADDS;
                        m_uiSubPhase = SUBPHASE_TALK;
                        m_uiTimer = 23000;
                        m_creature->SetSpeedRate(MOVE_FLIGHT, 3.5f, true);
                        m_creature->SetSpeedRate(MOVE_RUN, 3.5f, true);
                        m_creature->SetSpeedRate(MOVE_WALK, 3.5f, true);
                        m_pInstance->SetData(TYPE_LIGHT, LIGHT_RUNES);
                        return;
                    }
                    m_uiTimer = 1500;
                }else m_uiTimer -= uiDiff;
                break;
            }
            case PHASE_ADDS:
            {
                if (m_uiSubPhase == SUBPHASE_TALK)
                {
                    if (m_uiTimer <= uiDiff)
                    {
                        DoScriptText(SAY_AGGRO2, m_creature);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        DoSpawnAdds();
                        DoSpawnShell();
                        m_uiShellTimer = 30000;
                        m_uiSubPhase = 0;
                        m_uiTimer = 15000;
                    }else m_uiTimer -= uiDiff;
                    return;
                }

                //Arcane overload (bubble)
                if (m_uiShellTimer <= uiDiff)
                {
                    DoSpawnShell();
                    DoScriptText(SAY_ARCANE_OVERLOAD, m_creature);
                    m_uiShellTimer = 20000;
                }else m_uiShellTimer -= uiDiff;

                // Deep breath
                if (m_uiDeepBreathTimer <= uiDiff)
                {
                    DoScriptText(SAY_ARCANE_PULSE, m_creature);
                    DoScriptText(SAY_ARCANE_PULSE_WARN, m_creature);
                    if (pTrigger)
                        m_creature->CastSpell(pTrigger, SPELL_SURGE_OF_POWER_BREATH, true);

                    m_uiDeepBreathTimer = 60000;
                }else m_uiDeepBreathTimer -= uiDiff;

                // Arcane Storm
                if (m_uiArcaneStormTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
                    m_uiArcaneStormTimer = 10000 + urand(1000, 9000);
                }else m_uiArcaneStormTimer -= uiDiff;

                if (m_uiTimer <= uiDiff)
                {
                    if (!IsThereAnyAdd())
                    {
                        m_creature->StopMoving();
                        m_uiPhase = PHASE_DRAGONS;
                        m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM1;
                        DoScriptText(SAY_END_PHASE2, m_creature);
                        if (pTrigger)
                            pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_PRE, false);
                        m_uiTimer = 6500;
                        m_pInstance->SetData(TYPE_LIGHT, LIGHT_WORMHOLE);
                        return;
                    }
                    m_uiTimer = 5000;
                }else m_uiTimer -= uiDiff;
                break;
            }
            case PHASE_DRAGONS:
            {
                switch(m_uiSubPhase)
                {
                    case SUBPHASE_DESTROY_PLATFORM1:
                        if (m_uiTimer<= uiDiff)
                        {
                            //Destroy Platform
                            if (pTrigger)
                                pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFROM_BOOM, false);
                            if (pFloor)
                            {
                                pFloor->TakenDamage(100, m_creature);
                                pFloor->TakenDamage(100, m_creature);
                            }

                            //Knock up players
                            KnockUpPlayers();

                            //Despawn bubbles and discs
                            DespawnCreatures(NPC_ARCANE_OVERLOAD, 70.0f);
                            DespawnCreatures(NPC_HOVER_DISC, 70.0f, true);

                            m_uiTimer = 3500;
                            m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM2;
                            m_pInstance->SetData(TYPE_LIGHT, LIGHT_NEBULASKY);
                        }else m_uiTimer -= uiDiff;
                        return;
                    case SUBPHASE_DESTROY_PLATFORM2:
                        if (m_uiTimer<= uiDiff)
                        {
                            // Mount players
                            MountPlayers();

                            DoMovement(OtherLoc[0][0], OtherLoc[0][1], OtherLoc[0][2]);
                            m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM3;
                            DoScriptText(SAY_INTRO_PHASE3, m_creature);
                            m_uiTimer = 14900;
                        }else m_uiTimer -= uiDiff;
                        return;
                    case SUBPHASE_DESTROY_PLATFORM3:
                    {
                        if (m_uiTimer<= uiDiff)
                        {
                            m_uiSubPhase = 0;
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            SetCombatMovement(false);
                            m_creature->GetMotionMaster()->Clear(false, true);        // No moving!
                            m_creature->GetMotionMaster()->MoveIdle();
                            if (Unit *pVehicle = m_creature->GetMap()->GetVehicle(m_creature->getVictim()->GetVehicleGUID()))
                            {
                                float victim_threat = m_creature->getThreatManager().getThreat(m_creature->getVictim());
                                DoResetThreat();
                                m_creature->AI()->AttackStart(pVehicle);
                                m_creature->AddThreat(pVehicle, victim_threat);
                            }
                            if (pFloor)
                                pFloor->Delete();
                            DoScriptText(SAY_AGGRO3, m_creature);
                        }else m_uiTimer -= uiDiff;
                        return;
                    }
                    default: break;
                }

                //Arcane Pulse
                if (m_uiArcanePulseTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_ARCANE_PULSE);
                    m_uiArcanePulseTimer = 1000;
                }else m_uiArcanePulseTimer -= uiDiff;

                //Static field
                if (m_uiStaticFieldTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (Creature *pField = m_creature->SummonCreature(NPC_STATIC_FIELD, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 25000))
                            pField->CastSpell(pField, SPELL_STATIC_FIELD, true);
                    DoScriptText(SAY_CAST_SPELL1-urand(0,2), m_creature);
                    m_uiStaticFieldTimer = 10000+rand()%15000;
                }else m_uiStaticFieldTimer -= uiDiff;

                //Surge of power
                if (m_uiSurgeOfPowerTimer <= uiDiff)
                {
                    Unit* pTarget;
                    //Malygos has some triggers in aggro list in that phase :o
                    uint8 count = 0; // limit it to prevent lag
                    for(pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0); pTarget->GetEntry() != NPC_WYRMREST_SKYTALON; )
                    {
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                        ++count;
                        if (count >= 50)
                            break;
                    }
                    float victim_threat = m_creature->getThreatManager().getThreat(m_creature->getVictim());
                    DoResetThreat();
                    m_creature->AddThreat(pTarget, victim_threat);
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_SURGE_OF_POWER : SPELL_SURGE_OF_POWER_H);
                    DoScriptText(SAY_SURGE_OF_POWER, m_creature);
                    m_uiSurgeOfPowerTimer = 13000+rand()%15000;
                }else m_uiSurgeOfPowerTimer -= uiDiff;
                break;
            }
            case PHASE_OUTRO:
            {
                if (m_uiSubPhase == SUBPHASE_STOP_COMBAT)
                {
                    m_pInstance->SetData(TYPE_OUTRO_CHECK, 1);

                    if (m_creature->IsNonMeleeSpellCasted(false))
                        m_creature->InterruptNonMeleeSpells(false);

                    m_creature->RemoveAllAuras();
                    SetCombatMovement(false);

                    if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                        m_creature->GetMotionMaster()->MovementExpired();

                    DespawnCreatures(NPC_STATIC_FIELD, 120.0f);

                    m_creature->SetHealth(1);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->SetTargetGUID(0);
                    m_creature->GetMotionMaster()->Clear(false);        // No moving!
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_uiSpeechCount = 0;
                    m_uiSpeechTimer[0] = 2000;
                    m_uiSpeechTimer[1] = 8500;
                    m_uiSpeechTimer[2] = 5000;
                    m_uiSpeechTimer[3] = 3000;
                    m_uiSpeechTimer[4] = 22000;

                    if (Creature *pTemp = m_creature->SummonCreature(NPC_ALEXSTRASZA, OtherLoc[4][0], OtherLoc[4][1], OtherLoc[4][2], 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
                        pTemp->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
                        m_creature->SetOrientation(m_creature->GetAngle(pTemp));
                        m_creature->SendHeartBeatMsg();
                        pTemp->SetFacingToObject(m_creature);
                        pAlexstrasza = pTemp;

                        PointPath path;
                        path.resize(2);
                        path.set(0, Coords(OtherLoc[4][0], OtherLoc[4][1], OtherLoc[4][2]));
                        path.set(1, Coords(OtherLoc[3][0], OtherLoc[3][1], OtherLoc[3][2]));
                        pAlexstrasza->ChargeMonsterMove(path, SPLINETYPE_FACINGTARGET, SPLINEFLAG_FLYING, 5000, m_creature->GetGUID());
                    }

                    m_uiSubPhase = 0;
                    return;
                }
                if (m_uiSpeechCount >= 5 || m_uiSubPhase == SUBPHASE_DIE)
                    return;

                if (m_uiSpeechTimer[m_uiSpeechCount] <= uiDiff)
                {
                    Creature *pSpeaker = NULL;
                    if (m_uiSpeechCount == 0)
                        pSpeaker = m_creature;
                    else
                        pSpeaker = pAlexstrasza;

                    if (pSpeaker && pSpeaker->isAlive())
                        DoScriptText(SAY_OUTRO1-m_uiSpeechCount, pSpeaker);

                    switch(m_uiSpeechCount)
                    {
                        case 2:
                        {
                            WorldPacket data(SMSG_DESTROY_OBJECT, 9);
                            data << uint64(m_creature->GetGUID());
                            data << uint8(1);
                            m_creature->SendMessageToSet(&data, false);
                            break;
                        }
                        case 4:
                        {
                            m_uiSubPhase = SUBPHASE_DIE;
                            m_creature->SetVisibility(VISIBILITY_OFF);
                
                            //Summon exit portal, platform and loot
                            m_creature->SummonGameobject(GO_EXIT_PORTAL, GOPositions[2][0], GOPositions[2][1], GOPositions[2][2], GOPositions[2][3], 0);
                            //m_creature->SummonGameobject(GO_PLATFORM, GOPositions[0][0], GOPositions[0][1], GOPositions[0][2], GOPositions[0][3], 0);
                            if(GameObject *pGift = m_creature->SummonGameobject(m_bIsRegularMode ? GO_ALEXSTRASZAS_GIFT : GO_ALEXSTRASZAS_GIFT_H, OtherLoc[0][0], OtherLoc[0][1], OtherLoc[0][2]+15, GOPositions[1][3],604800))
                                pAlexstrasza->SetFacingToObject(pGift);
                            m_creature->LogKill(m_creature->getVictim());
                            m_creature->getVictim()->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            break;
                        }
                    }
                    ++m_uiSpeechCount;
                }else m_uiSpeechTimer[m_uiSpeechCount] -= uiDiff;
                return;
            }
        } // switch(m_uiPhase)
        DoMeleeAttackIfReady();
    }

    void PortalBeam(bool cancel)
    {
        if (cancel)
        {
            m_creature->SetChannelObjectGUID(0);
            m_creature->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);

            WorldPacket data( MSG_CHANNEL_UPDATE, 8+4 );
            data << m_creature->GetPackGUID();
            data << uint32(0);
            m_creature->SendMessageToSet(&data, false);

            data.Initialize(SMSG_SPELL_FAILURE, (8+4+1));
            data << m_creature->GetPackGUID();
            data << uint8(0);
            data << uint32(SPELL_PORTAL_BEAM);
            data << uint8(0);
            m_creature->SendMessageToSet(&data, false);

            data.Initialize(SMSG_SPELL_FAILED_OTHER, (8+4));
            data << m_creature->GetPackGUID();
            data << uint8(0);
            data << uint32(SPELL_PORTAL_BEAM);
            data << uint8(0);
            m_creature->SendMessageToSet(&data, false);

            if (pPortalDummy)
                pPortalDummy->SetDisplayId(pPortalDummy->GetNativeDisplayId());
        }
        else
        {
            pPortalDummy  = GetClosestCreatureWithEntry(m_creature, NPC_SPARK_PORTAL, 180.0f);
            if (!pPortalDummy)
                return;
            m_creature->SetFacingTo(m_creature->GetAngle(pPortalDummy));
            pPortalDummy->SetDisplayId(27393);

            WorldPacket data(SMSG_SPELL_START);
            data << m_creature->GetPackGUID();
            data << m_creature->GetPackGUID();
            data << uint8(0);       //cast count
            data << uint32(SPELL_PORTAL_BEAM);
            data << uint32(CAST_FLAG_UNKNOWN1 | CAST_FLAG_UNKNOWN12);
            data << uint32(0);      // timer
            data << uint32(TARGET_FLAG_UNIT);
            data << pPortalDummy->GetPackGUID();
            m_creature->SendMessageToSet(&data, false);

            data.Initialize(SMSG_SPELL_GO);
            data << m_creature->GetPackGUID();
            data << m_creature->GetPackGUID();
            data << uint8(0);       //cast count
            data << uint32(SPELL_PORTAL_BEAM);
            data << uint32(CAST_FLAG_UNKNOWN12 | CAST_FLAG_UNKNOWN3);
            data << uint32(getMSTime());      // timer
            data << uint8(1);
            data << pPortalDummy->GetGUID();
            data << uint8(0);
            data << uint32(TARGET_FLAG_UNIT);
            data << pPortalDummy->GetPackGUID();
            m_creature->SendMessageToSet(&data, false);

            data.Initialize(MSG_CHANNEL_START, (8+4+4));
            data << m_creature->GetPackGUID();
            data << uint32(SPELL_PORTAL_BEAM);
            data << uint32(0);
            m_creature->SendMessageToSet(&data, false);
            m_creature->SetChannelObjectGUID(pPortalDummy->GetGUID());
            m_creature->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_PORTAL_BEAM);
        }
    }
};

/*######
## mob_power_spark
######*/
struct MANGOS_DLL_DECL mob_power_sparkAI : public ScriptedAI
{
    mob_power_sparkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool isDead;
    uint32 m_uiCheckTimer;
    Creature *pMalygos;

    void Reset()
    {
        isDead = false;
        pMalygos = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MALYGOS));
        m_uiCheckTimer = 2500;
        m_creature->SendHeartBeatMsg();
    }

    void AttackStart(Unit */*pWho*/)
    {
        return;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {

        if (isDead || pDoneBy->GetTypeId() != TYPEID_PLAYER)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            isDead = true;
            uiDamage = 0;

            m_creature->RemoveAllAuras();

            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();

            m_creature->SetHealth(1);
            m_creature->CastSpell(m_creature, SPELL_POWER_SPARK_PLAYERS, false);
            m_creature->ForcedDespawn(60000);
            if (pMalygos && pMalygos->isAlive())
                pMalygos->AI()->DoAction(1);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCheckTimer <= uiDiff)
        {
            if (pMalygos && pMalygos->isAlive() && !isDead)
            {
                if(m_creature->IsWithinDist(pMalygos, 6.0f, false))
                {
                    pMalygos->AI()->DoAction(0);
                    m_creature->CastSpell(pMalygos, SPELL_POWER_SPARK, true);
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    isDead = true;
                }
            }
            m_uiCheckTimer = 1000;
        }else m_uiCheckTimer -= uiDiff;
    }
};

/*######
## mob_scion_of_eternity
######*/

struct MANGOS_DLL_DECL mob_scion_of_eternityAI : public ScriptedAI
{
    mob_scion_of_eternityAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiArcaneBarrageTimer;
    //Movement - they are moving in "circles"....
    uint32 m_uiMoveTimer;
    bool m_bClockWise;
    float m_fDistance;
    float m_fAngle;
    float destX;
    float destY;
    bool isMoving;

    Creature *m_disc;

    void Reset()
    {
        m_uiArcaneBarrageTimer = 5000 + rand()%15000;
        isMoving = false;
        float x, y, z;
        m_creature->GetPosition(x, y, z);
        m_disc = m_creature->SummonVehicle(NPC_HOVER_DISC, x, y, z, 0);
        if(!m_disc)
            m_disc = m_creature;
        else
        {
            m_creature->EnterVehicle((Vehicle*)m_disc, -1);
            m_disc->setFaction(m_creature->getFaction());
            m_disc->SetSpeedRate(MOVE_FLIGHT, 0.7f, true);
            m_disc->m_movementInfo.SetMovementFlags2(MOVEFLAG2_ALLOW_PITCHING);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        InitMovement();
    }
    void InitMovement()
    {
        destX = 0;
        destY = 0;
        m_bClockWise = urand(0,1) ? true : false;
        m_uiMoveTimer = 1000;
        m_fDistance = m_creature->GetDistance2d(OtherLoc[2][0], OtherLoc[2][1]); // From center of platform

        m_fAngle = m_creature->GetAngle(OtherLoc[2][0], OtherLoc[2][1]) + M_PI_F;
        m_fAngle = (m_fAngle <= 2*M_PI_F) ? m_fAngle : m_fAngle - 2 * M_PI_F;
    }

    void DoNextMovement()
    {
        // Moving in "circles", <3 numberz :P
        float m_fLenght = 2*M_PI_F*m_fDistance;
        float m_fRotateAngle = (2*M_PI_F) / m_fLenght; // Moving by 1y every 700ms

        PointPath path;
        uint16 max = ceil((2*M_PI_F)/m_fRotateAngle);
        if(max > 30)
            max = 30;
        path.resize(max+1);
        path.set(0, m_creature->GetPosition());
        m_uiMoveTimer = max/(double(m_disc->GetSpeed(MOVE_FLIGHT))*0.001);

        for(uint16 itr = 1; itr <= max; ++itr)
        {
            if (m_bClockWise)
                m_fAngle -= m_fRotateAngle;
            else
                m_fAngle += m_fRotateAngle;

            //because it cant be lower than 0 or bigger than 2*PI
            m_fAngle = (m_fAngle >= 0) ? m_fAngle : 2 * M_PI_F + m_fAngle;
            m_fAngle = (m_fAngle <= 2*M_PI_F) ? m_fAngle : m_fAngle - 2 * M_PI_F;

            destX = OtherLoc[2][0] + cos(m_fAngle)*m_fDistance;
            destY = OtherLoc[2][1] + sin(m_fAngle)*m_fDistance;

            MaNGOS::NormalizeMapCoord(destX);
            MaNGOS::NormalizeMapCoord(destY);
            path.set(itr, Coords(destX, destY, FLOOR_Z+10));
            //m_creature->SummonCreature(1, destX, destY, FLOOR_Z+10, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }

        m_disc->GetMotionMaster()->Clear(false, true);
        m_disc->ChargeMonsterMove(path, SPLINETYPE_NORMAL, SplineFlags(SPLINEFLAG_FLYING | SPLINEFLAG_CATMULLROM), m_uiMoveTimer+200);
    }

    void AttackStart(Unit *pWho)
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneBarrageTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                int32 bpoints0 = m_bIsRegularMode ? int32(BP_BARRAGE0) : int32(BP_BARRAGE0_H);
                m_creature->CastCustomSpell(pTarget, SPELL_ARCANE_BARRAGE, &bpoints0, 0, 0, false);
            }
            m_uiArcaneBarrageTimer = 3000 + rand()%19000;
        }else m_uiArcaneBarrageTimer -= uiDiff;

        if (m_uiMoveTimer <= uiDiff)
        {
            DoNextMovement();
        }else m_uiMoveTimer -= uiDiff;
    }
};

/*######
## mob_nexus_lord
######*/

struct MANGOS_DLL_DECL mob_nexus_lordAI : public ScriptedAI
{
    mob_nexus_lordAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Creature *m_disc;
    Unit *curTarget;

    void Reset()
    {
        float x, y, z;
        m_creature->GetPosition(x, y, z);
        m_disc = m_creature->SummonVehicle(NPC_HOVER_DISC, x, y, z, 0);
        if(!m_disc)
            m_disc = m_creature;
        else
        {
            m_creature->EnterVehicle((Vehicle*)m_disc, -1);
            m_disc->setFaction(m_creature->getFaction());
            m_disc->m_movementInfo.SetMovementFlags2(MOVEFLAG2_ALLOW_PITCHING);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void AttackStart(Unit *pWho)
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            curTarget = pWho;
            m_disc->GetMotionMaster()->MoveFollow(pWho, 0, 0);
            m_disc->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        }
    }

    void UpdateAI(const uint32)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || !m_disc)
            return;

        if(m_creature->getVictim() != curTarget)
        {
            curTarget = m_creature->getVictim();
            m_disc->GetMotionMaster()->Clear(false, true);
            m_disc->GetMotionMaster()->MoveFollow(curTarget, 0, 0);
            m_disc->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## go_focusing_iris
######*/

bool GOHello_go_focusing_iris(Player* pPlayer, GameObject* pGo)
{
    bool m_bIsRegularMode = pGo->GetMap()->IsRegularDifficulty();

    bool hasItem = false;
    if (m_bIsRegularMode)
    {
        if (pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS, 1) || pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1))
            hasItem = true;
    }else{
        if (pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1))
            hasItem = true;
    }
    if (!hasItem)
        return false;

    // beam effect
    if (Creature *pTrigger = GetClosestCreatureWithEntry(pGo, NPC_AOE_TRIGGER, 20.0f))
    {
        WorldPacket data(SMSG_SPELL_START);
        data << pTrigger->GetPackGUID();
        data << pTrigger->GetPackGUID();
        data << uint8(0);       //cast count
        data << uint32(61012);
        data << uint32(CAST_FLAG_UNKNOWN1);
        data << uint32(0);      // timer
        data << uint32(TARGET_FLAG_SELF);
        pGo->SendMessageToSet(&data, false);

        data.Initialize(SMSG_SPELL_GO);
        data << pTrigger->GetPackGUID();
        data << pTrigger->GetPackGUID();
        data << uint8(0);       //cast count
        data << uint32(61012);
        data << uint32(CAST_FLAG_UNKNOWN3 | CAST_FLAG_UNKNOWN13 | CAST_FLAG_UNKNOWN17);
        data << uint32(getMSTime());      // timer 
        data << uint8(0);
        data << uint8(0);
        data << uint32(TARGET_FLAG_DEST_LOCATION);
        data << uint8(0);
        float cx, cy,cz;
        pGo->GetPosition(cx, cy, cz);
        data << cx << cy << float(cz+10.0f);
        data << uint8(2);
        pGo->SendMessageToSet(&data, false);
    }

    Creature *pMalygos = pGo->GetMap()->GetCreature(pGo->GetInstanceData()->GetData64(NPC_MALYGOS));
    if (!pMalygos)
       pMalygos = GetClosestCreatureWithEntry(pGo, NPC_MALYGOS, 150.0f);
    if (pMalygos)
    {
        pMalygos->AI()->DoAction(2);
        pGo->Delete();
    }
    return false;
}
CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}
CreatureAI* GetAI_mob_power_spark(Creature* pCreature)
{
    return new mob_power_sparkAI(pCreature);
}

CreatureAI* GetAI_mob_scion_of_eternity(Creature* pCreature)
{
    return new mob_scion_of_eternityAI(pCreature);
}

CreatureAI* GetAI_mob_nexus_lord(Creature* pCreature)
{
    return new mob_nexus_lordAI(pCreature);
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_power_spark";
    newscript->GetAI = &GetAI_mob_power_spark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scion_of_eternity";
    newscript->GetAI = &GetAI_mob_scion_of_eternity;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nexus_lord";
    newscript->GetAI = &GetAI_mob_nexus_lord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_focusing_iris";
    newscript->pGOHello = &GOHello_go_focusing_iris;
    newscript->RegisterSelf();
}
