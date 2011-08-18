/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_anubarak_trial
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

// Anubarak - underground phase partially not worked, timers need correct
// Burrower - underground phase not implemented, buff not worked.
// Leecheng Swarm spell not worked - awaiting core support
// Anubarak spike aura worked only after 9750

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    SPELL_FREEZING_SLASH    = 66012, // phase 1,3
    SPELL_PENETRATING_COLD  = 66013, // phase 1,3
    SPELL_PURSUED           = 67574, // phase 2
    SPELL_PUSUING_SPIKES    = 65920, // phase 2
    SPELL_LEECHING_SWARM    = 66118, // phase 3
    //SPELL_LEECHING_HEAL     = 66125,
    //SPELL_LEECHING_DAMAGE   = 66240,

    SPELL_ROLLING_THROW     = 67730,
    SPELL_ROLLING_THROW_VEH = 67731,
    SPELL_SUBMERGE_0        = 53421,

    SPELL_BERSERK           = 26662,

    // npc
    // frost sphere
    NPC_FROST_SPHERE        = 34606,

    SPELL_PERMAFROST        = 66193,

    // burrower
    NPC_BURROWER            = 34607,

    SPELL_NERUBIAN_BURROWER = 66332, // phase 1,2, hc + phase 3
    SPELL_NERUB_BURR_PROC   = 66333,

    SPELL_EXPOSE_WEAKNESS   = 67847,
    SPELL_SPIDER_FRENZY     = 66129,
    SPELL_SUBMERGE_1        = 67322,
    SPELL_SHADOW_STRIKE     = 66134, // hero 10, hero 25
    SPELL_SUMMON_PLAYER     = 21150, // hero 25

    // scarab
    NPC_SCARAB              = 34605,

    SPELL_SUMMON_SCARAB     = 66339, // phase 2
    SPELL_SUMMON_SCARAB_PROC= 66340,

    SPELL_ACID_MANDIBLE     = 65775,
    SPELL_DETERMINATION     = 66092,
    SPELL_SCARAB_ACHI_10    = 68186, // 10 man
    SPELL_SCARAB_ACHI_25    = 68515, // 25 man

    // spike
    NPC_SPIKE               = 34660,
    SPELL_SPIKE_CALL        = 66169
};


void AddSC_boss_anubarak_trial()
{
    Script* newscript;

}
