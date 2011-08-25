/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_CRUSADER_H
#define DEF_CRUSADER_H

#include "precompiled.h"

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

    DESPAWN_TIME                = 300000
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
    Coords(559.257996f, 90.266197f, 395.122986f),  // 0 Barrent
    Coords(563.672974f, 139.571f, 393.837006f),    // 1 Center
    Coords(563.833008f, 208.544995f, 395.26f),     // 2 Backdoor
    Coords(577.347839f, 195.338888f, 395.14f),     // 3 - Right
    Coords(550.955933f, 195.338888f, 395.14f),     // 4 - Left
    Coords(575.042358f, 195.260727f, 395.137146f), // 5
    Coords(552.248901f, 195.331955f, 395.132658f), // 6
    Coords(573.342285f, 195.515823f, 395.135956f), // 7
    Coords(554.239929f, 195.825577f, 395.137909f), // 8
    Coords(571.042358f, 195.260727f, 395.137146f), // 9
    Coords(556.720581f, 195.015472f, 395.132658f), // 10
    Coords(569.534119f, 195.214478f, 395.139526f), // 11
    Coords(569.231201f, 195.941071f, 395.139526f), // 12
    Coords(558.811610f, 195.985779f, 394.671661f), // 13
    Coords(567.641724f, 195.351501f, 394.659943f), // 14
    Coords(560.633972f, 195.391708f, 395.137543f), // 15
    Coords(565.816956f, 195.477921f, 395.136810f), // 16
    Coords(563.549f, 152.474f, 394.393f),          // 17 - Lich king start
    Coords(563.547f, 141.613f, 393.908f),          // 18 - Lich king end
    Coords(787.932556f, 133.28978f, 142.612152f),  // 19 - Anub'arak start location
    Coords(618.157898f, 132.640869f, 139.559769f), // 20 - Anub'arak move point location
    Coords(508.104767f, 138.247345f, 395.128052f), // 21 - Fizzlebang start location
    Coords(586.060242f, 117.514809f, 394.314026f), // 22 - Dark essence 1
    Coords(541.602112f, 161.879837f, 394.587952f), // 23 - Dark essence 2
    Coords(541.021118f, 117.262932f, 395.314819f), // 24 - Light essence 1
    Coords(586.200562f, 162.145523f, 394.626129f), // 25 - Light essence 2
    Coords(563.833008f, 195.244995f, 394.585561f), // 26 - outdoor
    Coords(548.610596f, 139.807800f, 394.321838f), // 27 - fizzlebang end
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

#endif
