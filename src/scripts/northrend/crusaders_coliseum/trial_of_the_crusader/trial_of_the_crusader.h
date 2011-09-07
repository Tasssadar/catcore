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
    MAX_ENCOUNTER               = 6,

    TYPE_COUNTER                = 7,
    TYPE_CHAMPION_SPAWN_MASK    = 8,

    NPC_BARRENT                 = 34816,
    NPC_TIRION                  = 34996,
    NPC_FIZZLEBANG              = 35458,
    NPC_GARROSH                 = 34995,
    NPC_WRYNN                   = 34990,
    NPC_LICH_KING               = 35877,
    NPC_OUTRO_TIRION            = 36095,
    NPC_OUTRO_ARGENT_MAGE       = 36097,

    NPC_THRALL                  = 34994,
    NPC_PROUDMOORE              = 34992,
    NPC_TRIGGER                 = 22517,
    NPC_WILFRED_PORTAL_GROUND   = 35651,

    NPC_ICEHOWL                 = 34797,
    NPC_GORMOK                  = 34796,
    NPC_DREADSCALE              = 34799,
    NPC_ACIDMAW                 = 35144,

    NPC_JARAXXUS                = 34780,
    NPC_MISTRESS_OF_PAIN        = 34826,
    NPC_FELFLAME_INFERNAL       = 34815,

    NPC_LIGHTBANE               = 34497,
    NPC_DARKBANE                = 34496,
    NPC_DARK_ESSENCE            = 34567,
    NPC_LIGHT_ESSENCE           = 34568,
    NPC_CONCENTRATED_DARKNESS   = 34628,
    NPC_CONCENTRATED_LIGHT      = 34630,

    NPC_ANUBARAK                = 34564,

    GO_CRUSADERS_CACHE_10       = 195631,
    GO_CRUSADERS_CACHE_25       = 195632,
    GO_CRUSADERS_CACHE_10_H     = 195633,
    GO_CRUSADERS_CACHE_25_H     = 195635,

    GO_TRIBUTE_CHEST_10H_0      = 195665,
    GO_TRIBUTE_CHEST_10H_5      = 195666,
    GO_TRIBUTE_CHEST_10H_25     = 195667,
    GO_TRIBUTE_CHEST_10H_49     = 195668,

    GO_TRIBUTE_CHEST_25H_0      = 195669,
    GO_TRIBUTE_CHEST_25H_5      = 195670,
    GO_TRIBUTE_CHEST_25H_25     = 195671,
    GO_TRIBUTE_CHEST_25H_49     = 195672,

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

const uint32 Dispell[] = {65684, 67176, 67177, 67178, 65686, 67222, 67223, 67224, 67590, 67602, 67603, 67604};

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

    CHAMPION_HEALER_COUNT   = 4,
    CHAMPION_DPS_COUNT      = 10,
    CHAMPION_COUNT          = 14,
    CHAMPION_ALL_COUNT      = 16
};

enum FactionFCH
{
    FACTION_ALLIANCE        = 0,
    FACTION_HORDE           = 1,
    FACTION_COUNT           = 2
};

enum Says
{
    SAY_STAGE_0_01            = -1649070,
    SAY_STAGE_0_02            = -1649071,
    SAY_STAGE_0_03a           = -1649072,
    SAY_STAGE_0_03h           = -1649073,
    SAY_STAGE_0_04            = -1649074,
    SAY_STAGE_0_05            = -1649075,
    SAY_STAGE_0_06            = -1649076,
    SAY_STAGE_0_WIPE          = -1649077,
    SAY_STAGE_1_01            = -1649080,
    SAY_STAGE_1_02            = -1649081,
    SAY_STAGE_1_03            = -1649082,
    SAY_STAGE_1_04            = -1649083,
    SAY_STAGE_1_05            = -1649030, //INTRO Jaraxxus
    SAY_STAGE_1_06            = -1649084,
    SAY_STAGE_1_07            = -1649086,
    SAY_STAGE_1_08            = -1649087,
    SAY_STAGE_1_09            = -1649088,
    SAY_STAGE_1_10            = -1649089,
    SAY_STAGE_1_11            = -1649090,
    SAY_STAGE_2_01            = -1649091,
    SAY_STAGE_2_02a           = -1649092,
    SAY_STAGE_2_02h           = -1649093,
    SAY_STAGE_2_03            = -1649094,
    SAY_STAGE_2_04a           = -1649095,
    SAY_STAGE_2_04h           = -1649096,
    SAY_STAGE_2_05a           = -1649097,
    SAY_STAGE_2_05h           = -1649098,
    SAY_STAGE_2_06            = -1649099,
    SAY_STAGE_3_01            = -1649100,
    SAY_STAGE_3_02            = -1649101,
    SAY_STAGE_3_03a           = -1649102,
    SAY_STAGE_3_03h           = -1649103,
    SAY_STAGE_4_01            = -1649104,
    SAY_STAGE_4_02            = -1649105,
    SAY_STAGE_4_03            = -1649106,
    SAY_STAGE_4_04            = -1649107,
    SAY_STAGE_4_05            = -1649108,
    SAY_STAGE_4_06            = -1649109,
    SAY_STAGE_4_07            = -1649110
};

#define REALLY_BIG_COOLDOWN 3600000

const uint32 FChampIDs[CHAMPION_ALL_COUNT][FACTION_COUNT] =
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

const Coords Center(563.672974f, 139.571f, 396.837006f);

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
    Coords(563.670f, 158.877f, 394.321f), // 27 - fizzlebang end
    Coords(563.611f, 140.205f, 393.903f), // 28 - chest loc
    Coords(563.670f, 125.817f, 394.321f), // 29 - jaroxxus portal leave
    Coords(510.910f, 110.557f, 418.234f), // 30 - horde champ spawn 1
    Coords(510.910f, 168.593f, 418.234f), // 31 - horde champ spawn 2
    Coords(522.718f, 118.555f, 395.210f), // 32 - horde champ jump 1
    Coords(522.995f, 160.063f, 395.024f), // 33 - horde champ jump 2
    Coords(537.764f, 139.571f, 394.391f), // 34 - horde champ loc
    Coords(617.262f, 168.593f, 418.234f), // 35 - ali champ spawn 1
    Coords(617.262f, 110.557f, 418.234f), // 36 - ali champ spawn 2
    Coords(605.405f, 160.063f, 395.209f), // 37 - ali champ jump 1
    Coords(605.405f, 118.555f, 395.210f), // 38 - ali champ jump 2
    Coords(590.654f, 139.571f, 394.393f), // 39 - ali champ loc
    Coords(571.833f, 170.544f, 403.209f), // 40 - twin light step 1
    Coords(593.654f, 170.544f, 403.209f), // 41 - twin light step 2
    Coords(593.654f, 139.571f, 403.209f), // 42 - twin light step 3
    Coords(555.833f, 170.544f, 403.209f), // 43 - twin dark step 1
    Coords(534.764f, 170.544f, 403.209f), // 44 - twin dark step 2
    Coords(534.764f, 139.571f, 403.209f), // 45 - twin dark step 3
    Coords(563.834f, 172.456f, 394.387f), // 46 - lich king spawn
    Coords(606.294f, 133.892f, 138.479f), // 47 - tirion down spawn
    Coords(648.916f, 131.021f, 141.616f), // 48 - tirion down final
    Coords(605.638f, 143.839f, 138.511f), // 49 - argent mage down spawn
    Coords(649.161f, 142.039f, 141.306f), // 50 - argent mage down final
    Coords(616.786f, 137.686f, 139.464f), // 51 - announcer down
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
    //POINT_LIGHT_1           = 110,
    //POINT_LIGHT_2           = 111,
    //POINT_LIGHT_3           = 112,
    //POINT_DARK_1            = 120,
    //POINT_DARK_2            = 121,
    //POINT_DARK_3            = 122,

    NUM_MESSAGES            = 5,

    SPELL_WILFRED_PORTAL    = 68424,
    SPELL_JARAXXUS_CHAINS   = 67924,
    SPELL_EMERGE_ACIDMAW    = 66947,
    SPELL_FEL_LIGHTNING_IK  = 67888,  // Fel Lightning - gnom instakill
    SPELL_LK_GATE           = 50795,
    SPELL_LK_NOVA           = 68198,

    SPELL_BERSERK           = 26662,    

    SAY_AGGRO_JARAXXUS      = -1649031
};

struct MANGOS_DLL_DECL npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature);

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHeroic;
    bool is10Man;

    int32       currentEncounter;
    uint16      encounterStage;
    uint32      customValue;
    Creature*   encounterCreature;
    Creature*   encounterCreature2;

    void Reset();

    void AttackStart(Unit* /*who*/);

    void ChooseEvent(uint8 encounterId, Player* chooser = NULL);

    void DataSet(uint32 type, uint32 data);

    void MovementInform(uint32 uiType, uint32 uiPointId);

    Creature* DoSpawnTocBoss(uint32 id, Coords coord, float ori, bool update_z = true);

    void SummonToCBoss(uint32 id, uint32 id2 = 0);

    void UpdateAI(const uint32 /*diff*/);

    void DeleteCreaturesAndRemoveAuras();

};

#endif
