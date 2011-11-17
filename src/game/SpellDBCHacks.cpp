/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "Common.h"
#include "Unit.h"
#include "SpellMgr.h"
#include "SharedDefines.h"
#include "DBCStores.h"
#include "SpellAuraDefines.h"

void SpellMgr::ApplySpellHacks()
{
    SpellEntry *sfix = NULL;

    //Shaman totem bar hack-fix
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66842));
        sfix->EffectImplicitTargetA[0] = TARGET_SELF;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66843));
        sfix->EffectImplicitTargetA[0] = TARGET_SELF;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66844));
        sfix->EffectImplicitTargetA[0] = TARGET_SELF;

    //Surge of power spells should be longer
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57407));
        sfix->DurationIndex = 28;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(60936));
        sfix->DurationIndex = 28;

    //Lifebloom final heal
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(33778));
        sfix->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;

    //Spirit of Redemption has AURA_INTERRUPT_FLAG_CAST, what the...? Maybe blizz changed this flag
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(27827));
        sfix->AuraInterruptFlags = 0;

    //Twilight Torment - relly dunno what blizzard intended to do
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57935));
        sfix->AttributesEx = 0;
        sfix->AttributesEx4 = SPELL_ATTR_EX4_NOT_STEALABLE;
        sfix->CastingTimeIndex = 1;
        sfix->RecoveryTime = 0;
        sfix->procFlags = (PROC_FLAG_TAKEN_MELEE_HIT | PROC_FLAG_TAKEN_MELEE_SPELL_HIT | PROC_FLAG_TAKEN_RANGED_HIT | PROC_FLAG_TAKEN_RANGED_SPELL_HIT | PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT);
        sfix->procChance = 100;
        sfix->procCharges = 0;
        sfix->rangeIndex = 1;
        sfix->StackAmount = 0;
        sfix->Effect[EFFECT_INDEX_1] = 0;
        sfix->EffectDieSides[EFFECT_INDEX_1] = 0;
        sfix->EffectBasePoints[EFFECT_INDEX_0] = -1;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 6;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
        sfix->EffectRadiusIndex[EFFECT_INDEX_0] = 0;
        sfix->EffectRadiusIndex[EFFECT_INDEX_1] = 0;
        sfix->EffectApplyAuraName[EFFECT_INDEX_0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sfix->EffectApplyAuraName[EFFECT_INDEX_1] = 0;
        sfix->EffectAmplitude[EFFECT_INDEX_0] = 0;
        sfix->EffectAmplitude[EFFECT_INDEX_1] = 0;
        sfix->EffectMiscValue[EFFECT_INDEX_0] = 0;
        sfix->EffectMiscValue[EFFECT_INDEX_1] = 0;
        sfix->EffectMiscValueB[EFFECT_INDEX_0] = 0;
        sfix->EffectMiscValueB[EFFECT_INDEX_1] = 0;
        sfix->EffectTriggerSpell[EFFECT_INDEX_0] = 57988;
        sfix->EffectTriggerSpell[EFFECT_INDEX_1] = 0;

    //Hurl Boulder
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62307));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Hurl Pyrite boulder
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62489));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = 16;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Anti-air rocket
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62363));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    
    //Mortar
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62635));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = 52;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    //Fire cannon 62357
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62357));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Blue pyrite
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68605));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 8;

    //Flames
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65045));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 8;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    // Searing Light - implicit target change
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63023));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;

    // Searing Light (h) - implicit target change
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65120));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;

    // Gravity Bomb - implicit target change
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63025));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 16;

    // Gravity Bomb (h) - implicit target change
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64233));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = 53;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 16;

    // Searing Flames - make it interruptable spell
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62661));
        sfix->InterruptFlags = 13;
    
    // Supercharge - should hit only allies
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(61920));
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 31;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 31;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_2] = 31;

    // Stone Grip
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62166));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_2] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    // Stone Grip (h)
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63981));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetA[EFFECT_INDEX_2] = TARGET_CHAIN_DAMAGE;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    // Slam triggered - needs to set rage requierement to 0 otherwise total cost is 30 rage instead of 15...
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(50782));
        sfix->powerType = 0;
        sfix->manaCost = 0;
        sfix->CastingTimeIndex = 0;

    // Item - Warrior T8 Protection 4P Bonus bonus triggered - spell icon and spell name
    /*sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64934));
        sfix->SpellIconID = 2097;
        sfix->SpellName[0] = "Hustej spell co snizi magical spell dymydz";*/

    // Fish Feast
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57292));
        sfix->EffectTriggerSpell[EFFECT_INDEX_2] = 57399;

    // Giddyup!
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(42924));
        sfix->EffectAmplitude[EFFECT_INDEX_0] = 1000;

    // Giddyup!
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(42436));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_0] = 21;

    // Ice Block - probably wrong mechanic, disabling its usage while afflicted by Forbearance
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(45438));
        sfix->Mechanic = 0;

    // Bladestorm, zkurvena immunita na nulu...
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(46924));
        sfix->EffectMiscValue[1]= 0;

    // magnetic core nechce se mì s tim srat v scriptu takže nasrat a rozmazat
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64436));
        sfix->EffectImplicitTargetA[EFFECT_INDEX_1]= 1;
        sfix->EffectImplicitTargetB[EFFECT_INDEX_1]= 0;

    // Fan of knives - rychlost (ma to bejt hned je to jakysi zpoždìny)
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(51723));
        sfix->speed = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(52874));
        sfix->speed = 0;

    // Forbearance
    // in DBC wrong mechanic immune since 3.0.x
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(25771));
        sfix->EffectMiscValue[0] = MECHANIC_IMMUNE_SHIELD;

    // Soulstorm - remove root effect, it does not work and causes trouble
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68872));
        sfix->Effect[2] = 0;

    // Brightleaf's Essence - why the fuck has this spell two exactly same auras ?
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62968));
        sfix->Effect[1] = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65761));
        sfix->Effect[1] = 0;

    // Wrong target type...
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68987));
        sfix->EffectImplicitTargetA[0] = 6;
        sfix->EffectImplicitTargetA[0] = 0;
        sfix->EffectImplicitTargetA[1] = 6;
        sfix->EffectImplicitTargetB[1] = 0;
        sfix->EffectImplicitTargetA[2] = 1;

    // Wrong target type...
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69232));
        sfix->EffectTriggerSpell[0] = 69233;
        sfix->EffectTriggerSpell[1] = 69238;

    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69238));
        sfix->EffectImplicitTargetA[0] = 28;
        sfix->EffectImplicitTargetB[0] = 0;
        sfix->EffectImplicitTargetA[1] = 28;
        sfix->EffectImplicitTargetB[1] = 0;

    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69628));
        sfix->EffectImplicitTargetA[0] = 28;
        sfix->EffectImplicitTargetB[0] = 0;
        sfix->EffectImplicitTargetA[1] = 28;
        sfix->EffectImplicitTargetB[1] = 0;

    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65509));
        sfix->Effect[1] = 6;
        sfix->EffectImplicitTargetA[1] = 22;
        sfix->EffectImplicitTargetB[1] = 7;
        sfix->EffectBasePoints[1] = 1900;
        sfix->EffectRadiusIndex[1] = 29;
        sfix->EffectApplyAuraName[1] = 3;
        sfix->EffectAmplitude[1] = 2000;

    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64598));
        sfix->Effect[0] = 6;
        sfix->Effect[1] = 0;
        sfix->Effect[2] = 0;
        sfix->EffectApplyAuraName[0] = 4;

    // Stonebark's Essence - remove visual effect
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62385));
        sfix->EffectTriggerSpell[2] = 0;
    sfix  = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65585));
        sfix->EffectTriggerSpell[2] = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62387));
        sfix->EffectTriggerSpell[2] = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65586));
        sfix->EffectTriggerSpell[2] = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62386));
        sfix->EffectTriggerSpell[1] = 0;
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65590));
        sfix->EffectTriggerSpell[1] = 0;

    // Item - Druid T8 Balance 4P Bonus
    sfix = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64823));
        sfix->procCharges = 1;
}
