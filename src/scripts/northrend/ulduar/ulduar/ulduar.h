#ifndef ULDUAR_H
#define ULDUAR_H

enum
{
    MAX_ENCOUNTER               = 14,
    HARD_ENCOUNTER              = 9,
    KEEPER_ENCOUNTER            = 4,

    // boss types
    TYPE_LEVIATHAN              = 0,
    TYPE_IGNIS                  = 1,
    TYPE_RAZORSCALE             = 2,
    TYPE_XT002                  = 3,
    TYPE_ASSEMBLY               = 4,
    TYPE_KOLOGARN               = 5,
    TYPE_AURIAYA                = 6,
    TYPE_MIMIRON                = 7,
    TYPE_HODIR                  = 8,
    TYPE_THORIM                 = 9,
    TYPE_FREYA                  = 10,
    TYPE_VEZAX                  = 11,
    TYPE_YOGGSARON              = 12,
    TYPE_ALGALON                = 13,

    DATA_STEELBREAKER           = 20,
    DATA_MOLGEIM                = 21,
    DATA_BRUNDIR                = 22,

    // hard mode bosses
    TYPE_LEVIATHAN_HARD         = 37,
    TYPE_XT002_HARD             = 38,
    TYPE_ASSEMBLY_HARD          = 39,
    TYPE_MIMIRON_HARD           = 40,
    TYPE_HODIR_HARD             = 41,
    TYPE_THORIM_HARD            = 42,
    TYPE_FREYA_HARD             = 43,
    TYPE_VEZAX_HARD             = 44,
    TYPE_YOGGSARON_HARD         = 45,

    // keepers help at Yogg
    TYPE_KEEPER_HODIR           = 46,
    TYPE_KEEPER_FREYA           = 47,
    TYPE_KEEPER_THORIM          = 48,
    TYPE_KEEPER_MIMIRON         = 49,

    // teleporters
    TYPE_LEVIATHAN_TP           = 50,
    TYPE_XT002_TP               = 51,
    TYPE_MIMIRON_TP             = 52,

    //other> these won't be saved to db
    TYPE_RUNE_GIANT             = 14,
    TYPE_RUNIC_COLOSSUS         = 15,
    TYPE_LEVIATHAN_MK           = 16,
    TYPE_VX001                  = 17,
    TYPE_AERIAL_UNIT            = 18,
    TYPE_YOGG_BRAIN             = 22,
    TYPE_MIMIRON_PHASE          = 23,
    TYPE_YOGG_PHASE             = 24,
    TYPE_VISION_PHASE           = 25,

    // siege
    NPC_LEVIATHAN               = 33113,
    NPC_IGNIS                   = 33118,
    NPC_RAZORSCALE              = 33186,
    NPC_COMMANDER               = 33210,
    NPC_XT002                   = 33293,

    // antechamber
    NPC_STEELBREAKER            = 32867,
    NPC_MOLGEIM                 = 32927,
    NPC_BRUNDIR                 = 32857,
    NPC_KOLOGARN                = 32930,
    NPC_RIGHT_ARM               = 32934,
    NPC_LEFT_ARM                = 32933,
    NPC_AURIAYA                 = 33515,
    NPC_SANCTUM_SENTRY          = 34014,
    NPC_FERAL_DEFENDER          = 34035,

    // keepers
    NPC_MIMIRON                 = 33350,
    NPC_LEVIATHAN_MK            = 33432,
    NPC_VX001                   = 33651,
    NPC_AERIAL_UNIT             = 33670,
    NPC_HODIR                   = 32845,
    NPC_THORIM                  = 32865,
    NPC_RUNIC_COLOSSUS          = 32872,
    NPC_RUNE_GIANT              = 32873,
    NPC_JORMUNGAR_BEHEMOTH      = 32882,
    NPC_FREYA                   = 32906,
    NPC_BRIGHTLEAF              = 32915,
    NPC_IRONBRACH               = 32913,
    NPC_STONEBARK               = 32914,

    // madness
    NPC_VEZAX                   = 33271,
    NPC_ANIMUS                  = 33524,
    NPC_YOGGSARON               = 33288,
    NPC_ALGALON                 = 32871,
    NPC_SARA                    = 33134,
    NPC_YOGG_BRAIN              = 33890, 

    // keepers images used to start the encounter
    NPC_THORIM_IMAGE            = 33413,
    NPC_MIMIRON_IMAGE           = 33412,
    NPC_HODIR_IMAGE             = 33411,
    NPC_FREYA_IMAGE             = 33410,

    // Keepers used at yogg saron encounter
    KEEPER_FREYA                = 33241,
    KEEPER_HODIR                = 33213,
    KEEPER_MIMIRON              = 33244,
    KEEPER_THORIM               = 33242,

    // loot chests
    // Kologarn
    GO_CACHE_OF_LIVING_STONE    = 195046,
    GO_CACHE_OF_LIVING_STONE_H	 = 195047,

    // Hodir
    GO_CACHE_OF_WINTER          = 194307,
    GO_CACHE_OF_WINTER_H        = 194308, 
    GO_CACHE_OF_RARE_WINTER     = 194200,
    GO_CACHE_OF_RARE_WINTER_H   = 194201,

    // Thorim
    GO_CACHE_OF_STORMS          = 194312,
    GO_CACHE_OF_RARE_STORMS     = 194313,
    GO_CACHE_OF_STORMS_H        = 194314,
    GO_CACHE_OF_RARE_STORMS_H   = 194315,

    // Alagon
    GO_GIFT_OF_OBSERVER_H       = 194821,
    GO_GIFT_OF_OBSERVER         = 194822,
    GO_GIFT_OF_OBSERVER_HH      = 194823,   // unk

    // Freya > each chest is for a mode = more elders alive = more items in chest
    // not used, summoned by spell
    // 10 man
    GO_FREYA_GIFT               = 194324, //10 normal
    GO_FREYA_GIFT_1             = 194325, //10 1 elder
    GO_FREYA_GIFT_2             = 194326, //10 2 elders
    GO_FREYA_GIFT_3             = 194327, //10 3 elders

    // 25 man
    GO_FREYA_GIFT_H             = 194328, //25 normal
    GO_FREYA_GIFT_H_1           = 194329, //25 1 elder
    GO_FREYA_GIFT_H_2           = 194330, //25 2 elder
    GO_FREYA_GIFT_H_3           = 194331, //25 3 elders

    // Mimiron
    GO_CACHE_OF_INOV            = 194789,
    GO_CACHE_OF_INOV_H          = 194956,
    GO_CACHE_OF_INOV_HARD       = 194957,
    GO_CACHE_OF_INOV_HARD_H     = 194958,

    // doors
    // the siege
    GO_SHIELD_WALL              = 194416,
    GO_LEVIATHAN_GATE           = 194630,
    GO_XT002_GATE               = 194631,
    GO_BROKEN_HARPOON           = 194565,

    // archivum
    GO_KOLOGARN_BRIDGE			= 194232,
    GO_SHATTERED_DOOR           = 194553,
    GO_IRON_ENTRANCE_DOOR       = 194554,
    GO_ARCHIVUM_DOOR            = 194556,
    GO_ARCHIVUM_CONSOLE         = 194555,

    // planetarium: algalon
    GO_CELESTIAL_ACCES          = 194628,
    GO_CELESTIAL_ACCES_H        = 194752,
    GO_CELESTIAL_DOOR           = 194767,
    GO_CELESTIAL_DOOR_2         = 194911,
    GO_UNIVERSE_FLOOR_ARCHIVUM  = 194715,
    GO_UNIVERSE_FLOOR_CELESTIAL = 194716,
    GO_AZEROTH_GLOBE            = 194148,

    // the keepers
    // hodir
    GO_HODIR_EXIT               = 194634,
    GO_HODIR_ICE_WALL           = 194441,
    GO_HODIR_ENTER              = 194442,

    // mimiron
    GO_MIMIRON_TRAM             = 194675,
    GO_MIMIRON_BUTTON           = 194739,
    GO_MIMIRON_DOOR_1           = 194774,
    GO_MIMIRON_DOOR_2           = 194775,
    GO_MIMIRON_DOOR_3           = 194776,
    GO_MIMIRON_TEL1             = 194741,
    GO_MIMIRON_TEL2             = 194742,
    GO_MIMIRON_TEL3             = 194743,
    GO_MIMIRON_TEL4             = 194744,
    GO_MIMIRON_TEL5             = 194740,
    GO_MIMIRON_TEL6             = 194746,
    GO_MIMIRON_TEL7             = 194747,
    GO_MIMIRON_TEL8             = 194748,
    GO_MIMIRON_TEL9             = 194745,
    GO_MIMIRON_ELEVATOR         = 194749,

    // Thorim
    GO_DARK_IRON_PORTCULIS      = 194560,
    GO_RUNED_STONE_DOOR         = 194557,
    GO_THORIM_STONE_DOOR        = 194558,
    GO_LIGHTNING_DOOR           = 194905,
    GO_LIGHTNING_FIELD          = 194559,
    GO_DOOR_LEVER               = 194264,

    //Yogg
    GO_ANCIENT_GATE             = 194255,
    GO_VEZAX_GATE               = 194750,
    GO_YOGG_GATE                = 194773,
    GO_BRAIN_DOOR1              = 194635,
    GO_BRAIN_DOOR2              = 194636,
    GO_BRAIN_DOOR3              = 194637,

    ACHIEV_IRON_COUNCIL         = 2888,
    ACHIEV_IRON_COUNCIL_H       = 2889,

    ACHIEV_KEEPERS              = 2890,
    ACHIEV_KEEPERS_H            = 2891,

    ACHIEV_CELESTIAL_DEFENDER   = 3259,     // realm first algalon
    SPELL_ALGALON_ACHIEV_TRIGG  = 65184,
    ACHIEV_DEATHS_DEMISE        = 3117,     // realm first yogg

    // loot chests
    LOOT_XT002_HARD             = 1000003,
    LOOT_XT002_HARD_H           = 1000006,
    LOOT_ASSEMBLY               = 1000005,
    //LOOT_ASEEMBLY_HALF          =
    LOOT_ASEEMBLY_HARD          = 1000004,
    LOOT_ASSEMBLY_H             = 1000007,
    //LOOT_ASSEMBLY_HALF_H        =
    LOOT_ASSEMBLY_HARD_H        = 1000008,
    LOOT_KOLOGARN               = 195047,
    LOOT_KOLOGARN_H             = 195046,
    LOOT_FREYA                  = 194324,
    LOOT_FREYA_HARD             = 194325,
    LOOT_FREYA_H                = 194326,
    LOOT_FREYA_HARD_H           = 194327,
    LOOT_HODIR                  = 194308,
    LOOT_HODIR_HARD             = 194201,
    LOOT_HODIR_H                = 194307,
    LOOT_HODIR_HARD_H           = 194200,
    LOOT_MIMIRON                = 194789,
    LOOT_MIMIRON_HARD           = 1000000,
    LOOT_MIMIRON_H              = 1000009,
    LOOT_MIMIRON_HARD_H         = 1000010,
    LOOT_THORIM                 = 194312,
    LOOT_THORIM_HARD            = 1000001,
    LOOT_THORIM_H               = 194315,
    LOOT_THORIM_HARD_H          = 1000011,
    LOOT_VEZAX_HARD             = 1000002,
    LOOT_VEZAX_HARD_H           = 1000012,
};

/*
float const m_fLootXT002[4]     = {915.5f,   -11.3f, 418.5f, 3.02f}; //xt002 hard
float const m_fLootAssembly[4]  = {1520.0f,  112.2f, 427.3f, 6.27f}; //assembly
float const m_fLootAssemblyH[4] = {1520.8f,  126.7f, 427.3f, 6.27f}; //assembly hard
float const m_fLootKologarn[4]  = {1838.1f,  -34.8f, 448.8f, 5.06f}; //kologarn
float const m_fLootFreya[4]     = {2366.3f,  -52.7f, 424.7f, 3.14f}; //freya
float const m_fLootHodir[4]     = {1973.0f, -199.5f, 432.7f, 3.68f}; //hodir
float const m_fLootHodirH[4]    = {2032.7f, -196.4f, 432.7f, 4.71f}; //hodir hard
float const m_fLootMimiron[4]   = {2759.0f, 2575.6f, 364.3f, 5.32f}; //mimiron
float const m_fLootMimironH[4]  = {2758.9f, 2561.6f, 364.3f, 1.27f}; //mimiron hard
float const m_fLootThorim[4]    = {2120.5f, -284.5f, 419.5f, 5.97f}; //thorim
float const m_fLootThorimH[4]   = {2150.7f, -285.9f, 419.6f, 3.30f}; //thorim hard
float const m_fLootVezax[4]     = {1853.5f,   47.4f, 342.3f, 1.62f}; //vezax hard
*/

/*struct LootChestLoc
{
    float x, y, z,o;
};

struct LootChestLoc m_fPosition[]=
{
    {915.5f,   -11.3f, 418.5f, 3.02f}, //xt002 hard
    {1520.0f,  112.2f, 427.3f, 6.27f}, //assembly
    {1520.8f,  126.7f, 427.3f, 6.27f}, //assembly hard
    {1838.1f,  -34.8f, 448.8f, 5.06f}, //kologarn
    {2366.3f,  -52.7f, 424.7f, 3.14f}, //freya
    //{2366.3f, -52.7f, 424.7f, 3.14f}, //freya hard
    {1973.0f, -199.5f, 432.7f, 3.68f}, //hodir
    {2032.7f, -196.4f, 432.7f, 4.71f}, //hodir hard
    {2759.0f, 2575.6f, 364.3f, 5.32f}, //mimiron
    {2758.9f, 2561.6f, 364.3f, 1.27f}, //mimiron hard
    {2120.5f, -284.5f, 419.5f, 5.97f}, //thorim
    {2150.7f, -285.9f, 419.6f, 3.30f}, //thorim hard
    {1853.5f,   47.4f, 342.3f, 1.62f}  //vezax hard
};
enum PositionId
{
    POS_LOOT_XT002_HARD         = 0,
    POS_LOOT_ASSEMBLY           = 1,
    POS_LOOT_ASSEMBLY_HARD      = 2,
    POS_LOOT_KOLOGARN           = 3,
    POS_LOOT_FREYA              = 4,
    //POS_LOOT_FREYA_HARD         = 5,
    POS_LOOT_HODIR              = 5,
    POS_LOOT_HODIR_HARD         = 6,
    POS_LOOT_MIMIRON            = 7,
    POS_LOOT_MIMIRON_HARD       = 8,
    POS_LOOT_THORIM             = 9,
    POS_LOOT_THORIM_HARD        = 10,
    POS_LOOT_VEZAX_HARD         = 11,
};*/

#endif