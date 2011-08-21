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
    //Shaman totem bar hack-fix
    SpellEntry *sfix1 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66842));
    sfix1->EffectImplicitTargetA[0] = TARGET_SELF;
    SpellEntry *sfix2 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66843));
    sfix2->EffectImplicitTargetA[0] = TARGET_SELF;
    SpellEntry *sfix3 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(66844));
    sfix3->EffectImplicitTargetA[0] = TARGET_SELF;

    //Surge of power spells should be longer
    SpellEntry *sfix4 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57407));
    sfix4->DurationIndex = 28;
    SpellEntry *sfix5 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(60936));
    sfix5->DurationIndex = 28;

    //Lifebloom final heal
    SpellEntry *sfix6 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(33778));
    sfix6->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;

    //Spirit of Redemption has AURA_INTERRUPT_FLAG_CAST, what the...? Maybe blizz changed this flag
    SpellEntry *sfix7 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(27827));
    sfix7->AuraInterruptFlags = 0;

    //Twilight Torment - relly dunno what blizzard intended to do
    SpellEntry *sfix8 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57935));
    sfix8->AttributesEx = 0;
    sfix8->AttributesEx4 = SPELL_ATTR_EX4_NOT_STEALABLE;
    sfix8->CastingTimeIndex = 1;
    sfix8->RecoveryTime = 0;
    sfix8->procFlags = (PROC_FLAG_TAKEN_MELEE_HIT | PROC_FLAG_TAKEN_MELEE_SPELL_HIT | PROC_FLAG_TAKEN_RANGED_HIT | PROC_FLAG_TAKEN_RANGED_SPELL_HIT | PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT);
    sfix8->procChance = 100;
    sfix8->procCharges = 0;
    sfix8->rangeIndex = 1;
    sfix8->StackAmount = 0;
    sfix8->Effect[EFFECT_INDEX_1] = 0;
    sfix8->EffectDieSides[EFFECT_INDEX_1] = 0;
    sfix8->EffectBasePoints[EFFECT_INDEX_0] = -1;
    sfix8->EffectImplicitTargetA[EFFECT_INDEX_0] = 6;
    sfix8->EffectImplicitTargetA[EFFECT_INDEX_1] = 0;
    sfix8->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    sfix8->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
    sfix8->EffectRadiusIndex[EFFECT_INDEX_0] = 0;
    sfix8->EffectRadiusIndex[EFFECT_INDEX_1] = 0;
    sfix8->EffectApplyAuraName[EFFECT_INDEX_0] = SPELL_AURA_PROC_TRIGGER_SPELL;
    sfix8->EffectApplyAuraName[EFFECT_INDEX_1] = 0;
    sfix8->EffectAmplitude[EFFECT_INDEX_0] = 0;
    sfix8->EffectAmplitude[EFFECT_INDEX_1] = 0;
    sfix8->EffectMiscValue[EFFECT_INDEX_0] = 0;
    sfix8->EffectMiscValue[EFFECT_INDEX_1] = 0;
    sfix8->EffectMiscValueB[EFFECT_INDEX_0] = 0;
    sfix8->EffectMiscValueB[EFFECT_INDEX_1] = 0;
    sfix8->EffectTriggerSpell[EFFECT_INDEX_0] = 57988;
    sfix8->EffectTriggerSpell[EFFECT_INDEX_1] = 0;

    //Hurl Boulder
    SpellEntry *sfix9 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62307));
    sfix9->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
    sfix9->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
    sfix9->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Hurl Pyrite boulder
    SpellEntry *sfix10 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62489));
    sfix10->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
    sfix10->EffectImplicitTargetA[EFFECT_INDEX_1] = 16;
    sfix10->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Anti-air rocket
    SpellEntry *sfix11 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62363));
    sfix11->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
    sfix11->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    
    //Mortar
    SpellEntry *sfix12 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62635));
    sfix12->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
    sfix12->EffectImplicitTargetA[EFFECT_INDEX_1] = 52;
    sfix12->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
    sfix12->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    sfix12->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
    sfix12->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    //Fire cannon 62357
    SpellEntry *sfix13 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62357));
    sfix13->EffectImplicitTargetA[EFFECT_INDEX_0] = 16;
    sfix13->EffectImplicitTargetA[EFFECT_INDEX_2] = 16;
    sfix13->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    //Blue pyrite
    SpellEntry *sfix14 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68605));
    sfix14->EffectImplicitTargetA[EFFECT_INDEX_0] = 8;

    //Flames
    SpellEntry *sfix15 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65045));
    sfix15->EffectImplicitTargetA[EFFECT_INDEX_0] = 8;
    sfix15->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;

    // Searing Light - implicit target change
    SpellEntry *sfix16 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63023));
    sfix16->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
    sfix16->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;

    // Searing Light (h) - implicit target change
    SpellEntry *sfix17 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65120));
    sfix17->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
    sfix17->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;

    // Gravity Bomb - implicit target change
    SpellEntry *sfix18 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63025));
    sfix18->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
    sfix18->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;
    sfix18->EffectImplicitTargetA[EFFECT_INDEX_1] = 53;
    sfix18->EffectImplicitTargetB[EFFECT_INDEX_1] = 16;

    // Gravity Bomb (h) - implicit target change
    SpellEntry *sfix19 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64233));
    sfix19->EffectImplicitTargetA[EFFECT_INDEX_0] = 53;
    sfix19->EffectImplicitTargetB[EFFECT_INDEX_0] = 16;
    sfix19->EffectImplicitTargetA[EFFECT_INDEX_1] = 53;
    sfix19->EffectImplicitTargetB[EFFECT_INDEX_1] = 16;

    // Searing Flames - make it interruptable spell
    SpellEntry *sfix20 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62661));
    sfix20->InterruptFlags = 13;
    
    // Supercharge - should hit only allies
    SpellEntry *sfix21 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(61920));
    sfix21->EffectImplicitTargetB[EFFECT_INDEX_0] = 31;
    sfix21->EffectImplicitTargetB[EFFECT_INDEX_1] = 31;
    sfix21->EffectImplicitTargetB[EFFECT_INDEX_2] = 31;

    // Stone Grip
    SpellEntry *sfix22 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62166));
    sfix22->EffectImplicitTargetA[EFFECT_INDEX_0] = TARGET_CHAIN_DAMAGE;
    sfix22->EffectImplicitTargetA[EFFECT_INDEX_1] = TARGET_CHAIN_DAMAGE;
    sfix22->EffectImplicitTargetA[EFFECT_INDEX_2] = TARGET_CHAIN_DAMAGE;
    sfix22->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    sfix22->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
    sfix22->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    // Stone Grip (h)
    SpellEntry *sfix23 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(63981));
    sfix23->EffectImplicitTargetA[EFFECT_INDEX_0] = TARGET_CHAIN_DAMAGE;
    sfix23->EffectImplicitTargetA[EFFECT_INDEX_1] = TARGET_CHAIN_DAMAGE;
    sfix23->EffectImplicitTargetA[EFFECT_INDEX_2] = TARGET_CHAIN_DAMAGE;
    sfix23->EffectImplicitTargetB[EFFECT_INDEX_0] = 0;
    sfix23->EffectImplicitTargetB[EFFECT_INDEX_1] = 0;
    sfix23->EffectImplicitTargetB[EFFECT_INDEX_2] = 0;

    // Slam triggered - needs to set rage requierement to 0 otherwise total cost is 30 rage instead of 15...
    SpellEntry *sfix24 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(50782));
    sfix24->powerType = 0;
    sfix24->manaCost = 0;
    sfix24->CastingTimeIndex = 0;

    // Item - Warrior T8 Protection 4P Bonus bonus triggered - spell icon and spell name
    /*SpellEntry *sfix25 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64934));
    sfix25->SpellIconID = 2097;
    sfix25->SpellName[0] = "Hustej spell co snizi magical spell dymydz";*/

    // Fish Fest
    SpellEntry *sfix26 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(57292));
    sfix26->EffectTriggerSpell[EFFECT_INDEX_2] = 57399;

    // Giddyup!
    SpellEntry *sfix27 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(42924));
    sfix27->EffectAmplitude[EFFECT_INDEX_0] = 1000;

    // Giddyup!
    SpellEntry *sfix28 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(42436));
    sfix28->EffectImplicitTargetA[EFFECT_INDEX_0] = 21;

    // Ice Block - probably wrong mechanic, disabling its usage while afflicted by Forbearance
    SpellEntry *sfix29 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(45438));
    sfix29->Mechanic = 0;

    // Bladestorm, zkurvena immunita na nulu...
    SpellEntry *sfix30 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(46924));
    sfix30->EffectMiscValue[1]= 0;

    // magnetic core nechce se mì s tim srat v scriptu takže nasrat a rozmazat
    SpellEntry *sfix31 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64436));
    sfix31->EffectImplicitTargetA[EFFECT_INDEX_1]= 1;
    sfix31->EffectImplicitTargetB[EFFECT_INDEX_1]= 0;

    // Fan of knives - rychlost (ma to bejt hned je to jakysi zpoždìny)
    SpellEntry *sfix32 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(51723));
    sfix32->speed = 0;
    SpellEntry *sfix33 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(52874));
    sfix33->speed = 0;

    // Forbearance
    // in DBC wrong mechanic immune since 3.0.x
    SpellEntry *sfix34 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(25771));
    sfix34->EffectMiscValue[0] = MECHANIC_IMMUNE_SHIELD;

    // Soulstorm - remove root effect, it does not work and causes trouble
    SpellEntry *sfix35 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68872));
    sfix35->Effect[2] = 0;

    // Brightleaf's Essence - why the fuck has this spell two exactly same auras ?
    SpellEntry *sfix36 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(62968));
    sfix36->Effect[1] = 0;
    SpellEntry *sfix37 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65761));
    sfix37->Effect[1] = 0;

    // Wrong target type...
    SpellEntry *sfix38 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(68987));
    sfix38->EffectImplicitTargetA[0] = 6;
    sfix38->EffectImplicitTargetA[0] = 0;
    sfix38->EffectImplicitTargetA[1] = 6;
    sfix38->EffectImplicitTargetB[1] = 0;
    sfix38->EffectImplicitTargetA[2] = 1;

    
    // Wrong target type...
    SpellEntry *sfix39 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69232));
    sfix39->EffectTriggerSpell[0] = 69233;
    sfix39->EffectTriggerSpell[1] = 69238;

    SpellEntry *sfix40 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69238));
    sfix40->EffectImplicitTargetA[0] = 28;
    sfix40->EffectImplicitTargetB[0] = 0;
    sfix40->EffectImplicitTargetA[1] = 28;
    sfix40->EffectImplicitTargetB[1] = 0;

    SpellEntry *sfix41 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(69628));
    sfix41->EffectImplicitTargetA[0] = 28;
    sfix41->EffectImplicitTargetB[0] = 0;
    sfix41->EffectImplicitTargetA[1] = 28;
    sfix41->EffectImplicitTargetB[1] = 0;

    SpellEntry *sfix42 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(65509));
    sfix42->Effect[1] = 6;
    sfix42->EffectImplicitTargetA[1] = 22;
    sfix42->EffectImplicitTargetB[1] = 7;
    sfix42->EffectBasePoints[1] = 1900;
    sfix42->EffectRadiusIndex[1] = 29;
    sfix42->EffectApplyAuraName[1] = 3;
    sfix42->EffectAmplitude[1] = 2000;

    
    SpellEntry *sfix43 = const_cast<SpellEntry*>(sSpellStore.LookupEntry(64598));
    sfix43->Effect[0] = 6;
    sfix43->Effect[1] = 0;
    sfix43->Effect[2] = 0;
    sfix43->EffectApplyAuraName[0] = 4;
}
