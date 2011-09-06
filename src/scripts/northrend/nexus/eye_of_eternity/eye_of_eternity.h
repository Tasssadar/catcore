#ifndef DEF_EYE_OF_ETERNITY_H
#define DEF_EYE_OF_ETERNITY_H

enum
{
    MAX_ENCOUNTER                = 1,

    TYPE_MALYGOS                 = 2,
    TYPE_OUTRO_CHECK             = 4,  // Because this encounter is really hard and outro takes about 90s and malygos dies at its end, I add some check against server crash
    TYPE_LIGHT                   = 5,

    NPC_WYRMREST_SKYTALON        = 30161, // Dragon Vehicle in Third Phase
    NPC_MALYGOS                  = 28859,
    GO_ALEXSTRASZAS_GIFT         = 193905, // Loot chest
    GO_ALEXSTRASZAS_GIFT_H       = 193967, // Loot chest
    
    //Lights
    LIGHT_NORMAL                 = 1773,
    LIGHT_RUNES                  = 1824,
    LIGHT_WORMHOLE               = 1823,
    LIGHT_NEBULASKY              = 1822,
};
#endif
