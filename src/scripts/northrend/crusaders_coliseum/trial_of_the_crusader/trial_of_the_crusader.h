/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_CRUSADER_H
#define DEF_CRUSADER_H

#include "precompiled.h"

#define CHEST_ORI -1.58825
enum
{
    TYPE_BEASTS                 = 0,
    TYPE_JARAXXUS               = 1,
    TYPE_CRUSADERS              = 2,
    TYPE_VALKIRIES              = 3,
    TYPE_LICH_KING              = 4,
    TYPE_ANUBARAK               = 5,
    MAX_ENCOUNTER              = 6,

    TYPE_COUNTER                = 8,
    TYPE_EVENT                  = 9,

    NPC_BARRENT                 = 34816,
    NPC_TIRION                  = 34996,
    NPC_FIZZLEBANG              = 35458,
    NPC_GARROSH                 = 34995,
    NPC_RINN                    = 34990,
    NPC_LICH_KING_0             = 16980,
    NPC_LICH_KING_1             = 35877,

    NPC_THRALL                  = 34994,
    NPC_PROUDMOORE              = 34992,
    NPC_TRIGGER                 = 22517,
    NPC_WILFRED_PORTAL_GROUND   = 35651,

    NPC_ICEHOWL                 = 34797,
    NPC_GORMOK                  = 34796,
    NPC_DREADSCALE              = 34799,
    NPC_ACIDMAW                 = 35144,

    NPC_JARAXXUS                = 34780,

    NPC_CRUSADER_0_1            = 35465,
    NPC_CRUSADER_0_2            = 35610,

    NPC_LIGHTBANE               = 34497,
    NPC_DARKBANE                = 34496,

    NPC_ANUBARAK                = 34564,

    GO_CRUSADERS_CACHE_10       = 195631,
    GO_CRUSADERS_CACHE_25       = 195632,
    GO_CRUSADERS_CACHE_10_H     = 195633,
    GO_CRUSADERS_CACHE_25_H     = 195635,

    GO_TRIBUTE_CHEST_10H_25     = 195665,
    GO_TRIBUTE_CHEST_10H_45     = 195666,
    GO_TRIBUTE_CHEST_10H_50     = 195667,
    GO_TRIBUTE_CHEST_10H_99     = 195668,

    GO_TRIBUTE_CHEST_25H_25     = 195669,
    GO_TRIBUTE_CHEST_25H_45     = 195670,
    GO_TRIBUTE_CHEST_25H_50     = 195671,
    GO_TRIBUTE_CHEST_25H_99     = 195672,

    GO_ARGENT_COLISEUM_FLOOR    = 195527, //20943
    GO_MAIN_GATE_DOOR           = 195647,

    GO_WEST_PORTCULLIS          = 195589, // outside
    GO_SOUTH_PORTCULLIS         = 195590, // outside
    GO_NORTH_PORTCULLIS         = 195591, // outside

    GO_GATE_EAST                = 195648,
    GO_GATE_SOUTH               = 195649,
    GO_GATE_NORTH               = 195650,

    TYPE_DIFFICULTY             = 101,
    TYPE_EVENT_TIMER            = 102,
    TYPE_EVENT_NPC              = 103,
    TYPE_CRUSADERS_COUNT        = 105,

    DATA_HEALTH_EYDIS           = 201,
    DATA_HEALTH_FJOLA           = 202,
    DATA_CASTING_VALKYRS        = 203,

    DESPAWN_TIME                = 300000,

    POINT_TO_CENTER             = 231
};

enum Champion
{
    CHAMPION_DEATH_KNIGHT   = 0,
    CHAMPION_B_DRUID        = 1,
    CHAMPION_R_DRUID        = 2,
    CHAMPION_HUNTER         = 3,
    CHAMPION_MAGE           = 4,
    CHAMPION_H_PALADIN      = 5,
    CHAMPION_R_PALADIN      = 6,
    CHAMPION_D_PRIEST       = 7,
    CHAMPION_S_PRIEST       = 8,
    CHAMPION_ROGUE          = 9,
    CHAMPION_E_SHAMAN       = 10,
    CHAMPION_R_SHAMAN       = 11,
    CHAMPION_WARLOCK        = 12,
    CHAMPION_WARRIOR        = 13,
    CHAMPION_LOCK_PET       = 14,
    CHAMPION_HUNTER_PET     = 15,
    CHAMPION_COUNT          = 16
};

enum FactionFCH
{
    FACTION_ALLIANCE        = 0,
    FACTION_HORDE           = 1,
    FACTION_COUNT           = 2
};

const uint32 FChampIDs[CHAMPION_COUNT][FACTION_COUNT] =
{
    {34461, 34458},     // CHAMPION_DEATH_KNIGHT
    {34460, 34451},     // CHAMPION_B_DRUID
    {34469, 34459},     // CHAMPION_R_DRUID
    {34467, 34448},     // CHAMPION_HUNTER
    {34468, 34449},     // CHAMPION_MAGE
    {34465, 34445},     // CHAMPION_H_PALADIN
    {34471, 34456},     // CHAMPION_R_PALADIN
    {34466, 34447},     // CHAMPION_D_PRIEST
    {34473, 34441},     // CHAMPION_S_PRIEST
    {34472, 34454},     // CHAMPION_ROGUE
    {34463, 34455},     // CHAMPION_E_SHAMAN
    {34470, 34444},     // CHAMPION_R_SHAMAN
    {34474, 34450},     // CHAMPION_WARLOCK
    {34475, 34453},     // CHAMPION_WARRIOR
    {35465, 35465},     // CHAMPION_LOCK_PET
    {35610, 35610},     // CHAMPION_HUNT_PET
};

const Coords Center(563.672974f, 139.571f, 393.837006f);

const Coords SpawnLoc[] =
{
    Coords(559.257f,  90.266f, 395.122f), // 0 Barrent
    Coords(563.672f, 139.571f, 393.837f), // 1 Center
    Coords(563.833f, 208.544f, 395.222f), // 2 Backdoor
    Coords(577.347f, 195.338f, 395.146f), // 3 - Right
    Coords(550.955f, 195.338f, 395.146f), // 4 - Left
    Coords(575.042f, 195.260f, 395.137f), // 5
    Coords(552.248f, 195.331f, 395.132f), // 6
    Coords(573.342f, 195.515f, 395.135f), // 7
    Coords(554.239f, 195.825f, 395.137f), // 8
    Coords(571.042f, 195.260f, 395.137f), // 9
    Coords(556.720f, 195.015f, 395.132f), // 10
    Coords(569.534f, 195.214f, 395.139f), // 11
    Coords(569.231f, 195.941f, 395.139f), // 12
    Coords(558.811f, 195.985f, 394.671f), // 13
    Coords(567.641f, 195.351f, 394.659f), // 14
    Coords(560.633f, 195.391f, 395.137f), // 15
    Coords(565.816f, 195.477f, 395.136f), // 16
    Coords(563.549f, 152.474f, 394.393f), // 17 - Lich king start
    Coords(563.547f, 141.613f, 393.908f), // 18 - Lich king end
    Coords(787.932f, 133.289f, 142.612f), // 19 - Anub'arak start location
    Coords(618.157f, 132.640f, 139.559f), // 20 - Anub'arak move point location
    Coords(508.104f, 138.247f, 395.128f), // 21 - Fizzlebang start location
    Coords(586.060f, 117.514f, 394.314f), // 22 - Dark essence 1
    Coords(541.602f, 161.879f, 394.587f), // 23 - Dark essence 2
    Coords(541.021f, 117.262f, 395.314f), // 24 - Light essence 1
    Coords(586.200f, 162.145f, 394.626f), // 25 - Light essence 2
    Coords(563.833f, 195.244f, 394.585f), // 26 - outdoor
    Coords(548.610f, 139.807f, 394.321f), // 27 - fizzlebang end
    Coords(563.611f, 140.205f, 393.903f), // 28 - chest loc
    Coords(571.610f, 139.807f, 394.321f), // 29 - jaroxxus portal leave
};

enum uiWorldStates
{
    UPDATE_STATE_UI_SHOW            = 4390,
    UPDATE_STATE_UI_COUNT           = 4389
};

enum NorthrendBeasts
{
    GORMOK_IN_PROGRESS              = 5,
    GORMOK_DONE                     = 6,
    SNAKES_IN_PROGRESS              = 7,
    SNAKES_DONE                     = 8,
    ICEHOWL_IN_PROGRESS             = 9,
    ICEHOWL_DONE                    = 10
};

enum AnnounserMessages
{
    MSG_BEASTS                 = 724001,
    MSG_JARAXXUS               = 724002,
    MSG_CRUSADERS              = 724003,
    MSG_VALKIRIES              = 724004,
    MSG_LICH_KING              = 724005,
    MSG_ANUBARAK               = 724006
};

enum Announcer
{
    TIMER_PHASE_HANDLING    = 100,
    TIMER_DOOR_HANDLER      = 101,
    TIMER_RUNAWAY           = 102,
    TIMER_CUSTOM            = 103,

    POINT_PORT              = 100,

    NUM_MESSAGES            = 5,

    SPELL_WILFRED_PORTAL    = 68424,
    SPELL_JARAXXUS_CHAINS   = 67924,
    SPELL_EMERGE_ACIDMAW    = 66947,
    SPELL_FEL_LIGHTNING_IK  = 67888,  // Fel Lightning - gnom instakill

    SPELL_BERSERK           = 26662
};

struct MANGOS_DLL_DECL npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature);

    ScriptedInstance* m_pInstance;
    bool isHeroic;

    int32       currentEncounter;
    uint16      encounterStage;
    Creature*   encounterCreature;
    Creature*   encounterCreature2;

    void Reset();

    void AttackStart(Unit* /*who*/);

    void ChooseEvent(uint8 encounterId);

    void DataSet(uint32 type, uint32 data);

    void MovementInform(uint32 uiType, uint32 uiPointId);

    Creature* DoSpawnTocBoss(uint32 id, Coords coord, float ori);

    void SummonToCBoss(uint32 id, uint32 id2 = 0);

    void UpdateAI(const uint32 /*diff*/);

};

#endif
