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
SDName: Boss Sartharion
SD%Complete: 70%
SDComment: Flame wave, achievement and portal events need to be implemented
SDCategory: Obsidian Sanctum
EndScriptData */

#include "precompiled.h"
#include "obsidian_sanctum.h"

enum
{
    //Sartharion Yell
    SAY_SARTHARION_AGGRO                        = -1615018,
    SAY_SARTHARION_BERSERK                      = -1615019,
    SAY_SARTHARION_BREATH                       = -1615020,
    SAY_SARTHARION_CALL_SHADRON                 = -1615021,
    SAY_SARTHARION_CALL_TENEBRON                = -1615022,
    SAY_SARTHARION_CALL_VESPERON                = -1615023,
    SAY_SARTHARION_DEATH                        = -1615024,
    SAY_SARTHARION_SPECIAL_1                    = -1615025,
    SAY_SARTHARION_SPECIAL_2                    = -1615026,
    SAY_SARTHARION_SPECIAL_3                    = -1615027,
    SAY_SARTHARION_SPECIAL_4                    = -1615028,
    SAY_SARTHARION_SLAY_1                       = -1615029,
    SAY_SARTHARION_SLAY_2                       = -1615030,
    SAY_SARTHARION_SLAY_3                       = -1615031,

    WHISPER_LAVA_CHURN                          = -1615032,

    WHISPER_SHADRON_DICIPLE                     = -1615008,
    WHISPER_VESPERON_DICIPLE                    = -1615041,
    WHISPER_HATCH_EGGS                          = -1615017,
    WHISPER_OPEN_PORTAL                         = -1615042, // whisper, shared by two dragons

    //Sartharion Spells
    SPELL_BERSERK                               = 61632,    // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                                = 56909,    // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                          = 56908,    // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                        = 58956,    // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                             = 56910,    // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                           = 58957,    // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION                    = 61254,    // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_LAVA_STRIKE                           = 57571,    // (Real spell casted should be 57578) 57571 then trigger visual missile, then summon Lava Blaze on impact(spell 57572)
    SPELL_TWILIGHT_REVENGE                      = 60639,

    SPELL_PYROBUFFET                            = 56916,    // currently used for hard enrage after 15 minutes
    SPELL_ENRAGE                                = 64238,    // pyrobuffer does not work, so tipe them
    SPELL_PYROBUFFET_RANGE                      = 58907,    // possibly used when player get too far away from dummy creatures (2x creature entry 30494)

    SPELL_TWILIGHT_SHIFT_ENTER                  = 57620,    // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,    // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,    // leave phase (probably version to make all leave)
    SPELL_TWILIGHT_SHIFT_DAMAGE                 = 57874,

    //Mini bosses common spells
    SPELL_TWILIGHT_RESIDUE                      = 61885,    // makes immune to shadow damage, applied when leave phase

    //Miniboses (Vesperon, Shadron, Tenebron)
    SPELL_SHADOW_BREATH_H                       = 59126,    // Inflicts 8788 to 10212 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_BREATH                         = 57570,    // Inflicts 6938 to 8062 Fire damage to enemies in a cone in front of the caster.

    SPELL_SHADOW_FISSURE_H                      = 59127,    // Deals 9488 to 13512 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    SPELL_SHADOW_FISSURE                        = 57579,    // Deals 6188 to 8812 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    NPC_TWILIGHT_FISSURE                        = 30641,

    //Vesperon
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_VESPERON                     = 31219,    // Acolyte of Vesperon
    SPELL_POWER_OF_VESPERON                     = 61251,    // Vesperon's presence decreases the maximum health of all enemies by 25%.
    SPELL_TWILIGHT_TORMENT_VESP                 = 57948,    // (Shadow only) trigger 57935 then 57988
    SPELL_TWILIGHT_TORMENT_VESP_ACO             = 58853,    // (Fire and Shadow) trigger 58835 then 57988
    SPELL_TWILIGHT_TORMENT_PLAYERS              = 57935,

    //Shadron
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_SHADRON                      = 31218,    // Acolyte of Shadron
    SPELL_POWER_OF_SHADRON                      = 58105,    // Shadron's presence increases Fire damage taken by all enemies by 100%.
    SPELL_GIFT_OF_TWILIGTH_SHA                  = 57835,    // TARGET_SCRIPT shadron
    SPELL_GIFT_OF_TWILIGTH_SAR                  = 58766,    // TARGET_SCRIPT sartharion

    //Tenebron
    //in the portal spawns 6 eggs, if not killed in time (approx. 20s)  they will hatch,  whelps can cast 60708
    SPELL_POWER_OF_TENEBRON                     = 61248,    // Tenebron's presence increases Shadow damage taken by all enemies by 100%.
    //Tenebron, dummy spell
    SPELL_SUMMON_TWILIGHT_WHELP                 = 58035,    // doesn't work, will spawn NPC_TWILIGHT_WHELP
    SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP      = 58826,    // doesn't work, will spawn NPC_SHARTHARION_TWILIGHT_WHELP

    SPELL_HATCH_EGGS_H                          = 59189,
    SPELL_HATCH_EGGS                            = 58542,
    SPELL_HATCH_EGGS_EFFECT_H                   = 59190,
    SPELL_HATCH_EGGS_EFFECT                     = 58685,
    NPC_TWILIGHT_EGG                            = 30882,

    //Whelps
    NPC_TWILIGHT_WHELP                          = 30890,
    NPC_SHARTHARION_TWILIGHT_WHELP              = 31214,
    SPELL_FADE_ARMOR                            = 60708,    // Reduces the armor of an enemy by 1500 for 15s

    //flame tsunami
    SPELL_FLAME_TSUNAMI                         = 57494,    // the visual dummy
    SPELL_FLAME_TSUNAMI_LEAP                    = 60241,    // SPELL_EFFECT_138 some leap effect, causing caster to move in direction
    SPELL_FLAME_TSUNAMI_DMG_AURA                = 57492,    // periodic damage, npc has this aura
SPELL_KNOCKOUT = 37317,

    NPC_FLAME_TSUNAMI                           = 30616,    // for the flame waves
    NPC_LAVA_BLAZE                              = 30643,    // adds spawning from flame strike

    SPELL_SPHERE_VISUAL                         = 56075,    // Visual for immunity state, gift of twilight does not work gods know why

    //using these custom points for dragons start and end
    POINT_ID_INIT                               = 100,
    POINT_ID_LAND                               = 200
};

struct Waypoint
{
    float m_fX, m_fY, m_fZ;
};

struct Loc
{
    float x, y, z, o;
};
struct Achievements
{
    bool hardMode;
    uint8 drakesReq;
    uint32 ID;
};
//each dragons special points. First where fly to before connect to connon, second where land point is.
Waypoint m_aTene[]=
{
    {3212.854f, 575.597f, 109.856f},                        //init
    {3246.425f, 565.367f, 61.249f}                          //end
};

Waypoint m_aShad[]=
{
    {3293.238f, 472.223f, 106.968f},
    {3271.669f, 526.907f, 61.931f}
};

Waypoint m_aVesp[]=
{
    {3193.310f, 472.861f, 102.697f},
    {3227.268f, 533.238f, 59.995f}
};

//points around raid "isle", counter clockwise. should probably be adjusted to be more alike
Waypoint m_aDragonCommon[]=
{
    {3214.012f, 468.932f, 98.652f},
    {3244.950f, 468.427f, 98.652f},
    {3283.520f, 496.869f, 98.652f},
    {3287.316f, 555.875f, 98.652f},
    {3250.479f, 585.827f, 98.652f},
    {3209.969f, 566.523f, 98.652f}
};

Loc m_FlameTsunamiLoc[]=
{
    {3286.92f, 577.99f, 55.63f, 3.088f},  //North 1
    {3286.92f, 527.03f, 56.63f, 3.088f},  //North 2 
    {3286.92f, 483.11f, 56.63f, 3.088f},  //North 3 

    {3206.51f, 463.45f, 56.63f, 6.236f},  //South 1
    {3206.51f, 503.57f, 56.63f, 6.236f},  //South 2
    {3206.51f, 554.75f, 56.63f, 6.236f},  //South 3
};

Achievements m_DrakesAchi[]=
{
    {false, 1, 2049},
    {false, 2, 2050},
    {false, 3, 2051},
    {true, 1, 2052},
    {true, 2, 2053},
    {true, 3, 2054}
};
/*######
## Boss Sartharion
######*/

struct MANGOS_DLL_DECL boss_sartharionAI : public ScriptedAI
{
    boss_sartharionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
        Vesperon = NULL;
        Tenebron = NULL;
        Shadron = NULL;
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsBerserk;
    bool m_bIsSoftEnraged;

    uint32 m_uiEnrageTimer;
    bool m_bIsHardEnraged;
    uint8 m_uiLastTsunami;

    uint32 m_uiTenebronTimer;
    uint32 m_uiShadronTimer;
    uint32 m_uiVesperonTimer;

    uint32 m_uiFlameTsunamiTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiLavaStrikeTimer;

    uint32 m_uiPlayerMapRefresh;
    uint32 m_uiPlayerCheckTimer;
    uint32 m_uiPlayerDmgTimer;

    bool m_bHasCalledTenebron;
    bool m_bHasCalledShadron;
    bool m_bHasCalledVesperon;

    GuidList PlayerCheckList;

    Creature *Vesperon;
    Creature *Shadron;
    Creature *Tenebron;

    void Reset()
    {
        m_bIsBerserk = false;
        m_bIsSoftEnraged = false;

        m_uiEnrageTimer = MINUTE*15*IN_MILLISECONDS;
        m_bIsHardEnraged = false;
        m_uiLastTsunami = 0;

        m_uiTenebronTimer = 30000;
        m_uiShadronTimer = 75000;
        m_uiVesperonTimer = 120000;

        m_uiFlameTsunamiTimer = 30000;
        m_uiFlameBreathTimer = 20000;
        m_uiTailSweepTimer = 20000;
        m_uiCleaveTimer = 7000;
        m_uiLavaStrikeTimer = 5000;

        m_uiPlayerMapRefresh = 0;
        m_uiPlayerCheckTimer = 10000;

        m_bHasCalledTenebron = false;
        m_bHasCalledShadron = false;
        m_bHasCalledVesperon = false;

        PlayerCheckList.clear();

        if (m_creature->HasAura(SPELL_TWILIGHT_REVENGE))
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_REVENGE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, NOT_STARTED);

        if (Tenebron)
        {
            if (Tenebron->isAlive())
                Tenebron->AI()->EnterEvadeMode();
            else
                Tenebron->Respawn();
        }
        if (Shadron)
        {
            if (Shadron->isAlive())
                Shadron->AI()->EnterEvadeMode();
            else
                Shadron->Respawn();
        }
        if (Vesperon)
        {
            if (Vesperon->isAlive())
                Vesperon->AI()->EnterEvadeMode();
            else
                Vesperon->Respawn();
        }
        Vesperon = NULL;
        Tenebron = NULL;
        Shadron = NULL;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SARTHARION_AGGRO,m_creature);

        m_creature->SetInCombatWithZone();

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }
    }
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        //"Virtual" imunnity
        if (m_creature->HasAura(SPELL_SPHERE_VISUAL))
        {
            pDoneBy->SendSpellMiss(m_creature, 0, SPELL_MISS_IMMUNE);
            uiDamage = 0;
        }
    }
    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_SARTHARION_DEATH,m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, DONE);

        uint32 drakes = m_pInstance->GetData(TYPE_DRAGONS_ALIVE);
        for(int i = 0; i < 6; ++i)
        {
            if (m_DrakesAchi[i].hardMode != !m_bIsRegularMode || m_DrakesAchi[i].drakesReq > drakes)
                continue;

            // achievement hack
            if (pKiller->GetTypeId() == TYPEID_PLAYER)
                m_pInstance->CompleteAchievement(m_DrakesAchi[i].ID, (Player*)pKiller, true);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SARTHARION_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SARTHARION_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SARTHARION_SLAY_3, m_creature); break;
        }
    }

    void FetchDragons()
    {
        Unit* pTene = Unit::GetUnit(*m_creature,m_pInstance->GetData64(DATA_TENEBRON));
        Unit* pShad = Unit::GetUnit(*m_creature,m_pInstance->GetData64(DATA_SHADRON));
        Unit* pVesp = Unit::GetUnit(*m_creature,m_pInstance->GetData64(DATA_VESPERON));
        uint8 dragonsCount = 0;

        //if at least one of the dragons are alive and are being called
        bool bCanUseWill = false;

        if (pTene && pTene->isAlive() && !pTene->getVictim())
        {
            bCanUseWill = true;
            dragonsCount++;
            pTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].m_fX, m_aTene[0].m_fY, m_aTene[0].m_fZ);

            if (!pTene->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            ApplyAuraToDrake(pTene, SPELL_WILL_OF_SARTHARION);
            Tenebron = (Creature*)pTene;
        }

        if (pShad && pShad->isAlive() && !pShad->getVictim())
        {
            bCanUseWill = true;
            dragonsCount++;
            pShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].m_fX, m_aShad[0].m_fY, m_aShad[0].m_fZ);

            if (!pShad->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            ApplyAuraToDrake(pShad, SPELL_WILL_OF_SARTHARION);
            Shadron = (Creature*)pShad;
        }

        if (pVesp && pVesp->isAlive() && !pVesp->getVictim())
        {
            bCanUseWill = true;
            dragonsCount++;
            pVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].m_fX, m_aVesp[0].m_fY, m_aVesp[0].m_fZ);

            if (!pVesp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            ApplyAuraToDrake(pVesp, SPELL_WILL_OF_SARTHARION);
            Vesperon = (Creature*)pVesp;
        }

        if (bCanUseWill)
            DoCastSpellIfCan(m_creature, SPELL_WILL_OF_SARTHARION);

        m_pInstance->SetData(TYPE_DRAGONS_ALIVE, dragonsCount);
    }

    void CallDragon(uint32 uiDataId)
    {
        if (m_pInstance)
        {
            Creature* pTemp = (Creature*)Unit::GetUnit((*m_creature),m_pInstance->GetData64(uiDataId));

            if (pTemp && pTemp->isAlive() && !pTemp->getVictim())
            {
                if (pTemp->HasSplineFlag(SPLINEFLAG_WALKMODE))
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                int32 iTextId = 0;

                switch(pTemp->GetEntry())
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].m_fX, m_aTene[1].m_fY, m_aTene[1].m_fZ);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].m_fX, m_aShad[1].m_fY, m_aShad[1].m_fZ);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].m_fX, m_aVesp[1].m_fY, m_aVesp[1].m_fZ);
                        break;
                }

                DoScriptText(iTextId, m_creature);
            }
        }
    }

    void SendFlameTsunami()
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (i->getSource()->isAlive())
                        DoScriptText(WHISPER_LAVA_CHURN, m_creature,i->getSource());
                }
            }
        }
        //Summon it!
        m_uiLastTsunami = urand(0,1);
        for(int i = 0; i < 6; ++i)
        {
            if ((m_uiLastTsunami == 0 && i >= 3) || (m_uiLastTsunami == 1 && i < 3))
                continue;
            m_creature->SummonCreature(NPC_FLAME_TSUNAMI, m_FlameTsunamiLoc[i].x, m_FlameTsunamiLoc[i].y, m_FlameTsunamiLoc[i].z, m_FlameTsunamiLoc[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }
    }
    void ApplyAuraToDrake(Unit *pDragon, uint32 spellId)
    {
        if (!pDragon->isAlive())
            return;

        const SpellEntry* enrageProto = sSpellStore.LookupEntry(spellId);

        for (int i = 0; i < MAX_EFFECT_INDEX; i++)
        {           
            int32 basepoints = enrageProto->EffectBasePoints[SpellEffectIndex(i)];
            Aura* AurEff = CreateAura(enrageProto, SpellEffectIndex(i), &basepoints, pDragon, pDragon, NULL);
            pDragon->AddAura(AurEff);
        }
    }
    void CheckPlayers(const uint32 uiDiff)
    {
        if (m_uiPlayerMapRefresh < uiDiff)
        {
            PlayerCheckList.clear();
            Map::PlayerList const &list = m_creature->GetMap()->GetPlayers();
            for(Map::PlayerList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
            {
                if (!itr->getSource()->isAlive())
                    continue;
                PlayerCheckList.push_back(itr->getSource()->GetGUID());
            }

            m_uiPlayerMapRefresh = 30000;
        }
        else
            m_uiPlayerMapRefresh -= uiDiff;

        if (m_uiPlayerCheckTimer < uiDiff)
        {
            for(GuidList::iterator itr = PlayerCheckList.begin(); itr != PlayerCheckList.end(); ++itr)
            {
                Player *plr = (Player*)Unit::GetUnit(*m_creature, (*itr));
                if (!plr || !plr->isAlive() || !plr->IsInMap(m_creature))
                    continue;


                if (!(plr->GetPositionX() > 3218.86f && plr->GetPositionX() < 3275.69f &&
                    plr->GetPositionY() < 572.40f && plr->GetPositionY() > 484.68f))
                {
                    plr->CastSpell(plr, 69308, true, 0, 0, m_creature->GetGUID());
                }
            }
            m_uiPlayerCheckTimer = 5000;
        }
        else
            m_uiPlayerCheckTimer -= uiDiff;
    }
    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //spell will target dragons, if they are still alive at 35%
        if (!m_bIsBerserk && m_creature->GetHealthPercent() < 35.0f)
        {
            DoScriptText(SAY_SARTHARION_BERSERK, m_creature);
            //DoCastSpellIfCan(m_creature, SPELL_BERSERK);
            if (Tenebron && Tenebron->isAlive())
                ApplyAuraToDrake((Unit*)Tenebron, SPELL_BERSERK);
            if (Shadron && Shadron->isAlive())
                ApplyAuraToDrake((Unit*)Shadron, SPELL_BERSERK);
            if (Vesperon && Vesperon->isAlive())
                ApplyAuraToDrake((Unit*)Vesperon, SPELL_BERSERK);

            m_bIsBerserk = true;
        }

        //soft enrage
        if (!m_bIsSoftEnraged && m_creature->GetHealthPercent() <= 10.0f)
        {
            for(int i = 0; i < 10; ++i)
            {
                float angle = float(urand(0, 628)) / 100;
                float destX, destY, destZ;
                m_creature->GetPosition(destX, destY, destZ);

                float dist = float(urand(600, 4000)) / 1000;
                destX += cos(angle)*dist;
                destY += sin(angle)*dist;
                m_creature->UpdateGroundPositionZ(destX, destY, destZ);
                m_creature->SummonCreature(NPC_LAVA_BLAZE, destX, destY, destZ, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
            m_bIsSoftEnraged = true;
        }

        // hard enrage
        if (!m_bIsHardEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_ENRAGE, true);
                m_bIsHardEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        // flame tsunami
        if (m_uiFlameTsunamiTimer < uiDiff)
        {
            SendFlameTsunami();
            m_uiFlameTsunamiTimer = 30000;
        }
        else
            m_uiFlameTsunamiTimer -= uiDiff;

        // flame breath
        if (m_uiFlameBreathTimer < uiDiff)
        {
            DoScriptText(SAY_SARTHARION_BREATH, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H);
            m_uiFlameBreathTimer = urand(25000, 35000);
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TAIL_LASH : SPELL_TAIL_LASH_H);
            m_uiTailSweepTimer = urand(15000, 20000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(7000, 10000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Lavas Strike
        if (m_uiLavaStrikeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_LAVA_STRIKE);

                switch(urand(0, 15))
                {
                    case 0: DoScriptText(SAY_SARTHARION_SPECIAL_1, m_creature); break;
                    case 1: DoScriptText(SAY_SARTHARION_SPECIAL_2, m_creature); break;
                    case 2: DoScriptText(SAY_SARTHARION_SPECIAL_3, m_creature); break;
                }
            }
            m_uiLavaStrikeTimer = urand(5000, 20000);
        }
        else
            m_uiLavaStrikeTimer -= uiDiff;

        // call tenebron
        if (!m_bHasCalledTenebron && m_uiTenebronTimer < uiDiff)
        {
            CallDragon(DATA_TENEBRON);
            m_bHasCalledTenebron = true;
        }
        else
            m_uiTenebronTimer -= uiDiff;

        // call shadron
        if (!m_bHasCalledShadron && m_uiShadronTimer < uiDiff)
        {
            CallDragon(DATA_SHADRON);
            m_bHasCalledShadron = true;
        }
        else
            m_uiShadronTimer -= uiDiff;

        // call vesperon
        if (!m_bHasCalledVesperon && m_uiVesperonTimer < uiDiff)
        {
            CallDragon(DATA_VESPERON);
            m_bHasCalledVesperon = true;
        }
        else
            m_uiVesperonTimer -= uiDiff;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);

        CheckPlayers(uiDiff);
    }
};

CreatureAI* GetAI_boss_sartharion(Creature* pCreature)
{
    return new boss_sartharionAI(pCreature);
}

enum TeneText
{
    SAY_TENEBRON_AGGRO                      = -1615009,
    SAY_TENEBRON_SLAY_1                     = -1615010,
    SAY_TENEBRON_SLAY_2                     = -1615011,
    SAY_TENEBRON_DEATH                      = -1615012,
    SAY_TENEBRON_BREATH                     = -1615013,
    SAY_TENEBRON_RESPOND                    = -1615014,
    SAY_TENEBRON_SPECIAL_1                  = -1615015,
    SAY_TENEBRON_SPECIAL_2                  = -1615016
};

enum ShadText
{
    SAY_SHADRON_AGGRO                       = -1615000,
    SAY_SHADRON_SLAY_1                      = -1615001,
    SAY_SHADRON_SLAY_2                      = -1615002,
    SAY_SHADRON_DEATH                       = -1615003,
    SAY_SHADRON_BREATH                      = -1615004,
    SAY_SHADRON_RESPOND                     = -1615005,
    SAY_SHADRON_SPECIAL_1                   = -1615006,
    SAY_SHADRON_SPECIAL_2                   = -1615007
};

enum VespText
{
    SAY_VESPERON_AGGRO                      = -1615033,
    SAY_VESPERON_SLAY_1                     = -1615034,
    SAY_VESPERON_SLAY_2                     = -1615035,
    SAY_VESPERON_DEATH                      = -1615036,
    SAY_VESPERON_BREATH                     = -1615037,
    SAY_VESPERON_RESPOND                    = -1615038,
    SAY_VESPERON_SPECIAL_1                  = -1615039,
    SAY_VESPERON_SPECIAL_2                  = -1615040
};

//to control each dragons common abilities
struct MANGOS_DLL_DECL dummy_dragonAI : public ScriptedAI
{
    dummy_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWaypointId;
    uint32 m_uiMoveNextTimer;
    int32 m_iPortalRespawnTime;
    bool m_bCanMoveFree;
    uint32 m_uiPortalTimer;
    bool m_bHasSpawnedPortal;
    uint8 m_uiAcolytesCount;
    GuidList Acolytes;
    void Reset()
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_uiWaypointId = 0;
        m_uiMoveNextTimer = 500;
        m_iPortalRespawnTime = 30000;
        m_bCanMoveFree = false;
        m_uiPortalTimer = 30000;
        m_bHasSpawnedPortal = false;
        m_uiAcolytesCount = 0;
        Acolytes.clear();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!m_pInstance || uiType != POINT_MOTION_TYPE)
            return;

        debug_log("dummy_dragonAI: %s reached point %u", m_creature->GetName(), uiPointId);

        //if healers messed up the raid and we was already initialized
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        //this is the end (!)
        if (uiPointId == POINT_ID_LAND)
        {
            m_creature->GetMotionMaster()->Clear();
            m_bCanMoveFree = false;
            m_creature->SetInCombatWithZone();
            return;
        }

        //get amount of common points
        uint32 uiCommonWPCount = sizeof(m_aDragonCommon)/sizeof(Waypoint);

        //increase
        m_uiWaypointId = uiPointId+1;

        //if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= uiCommonWPCount)
        {
            if (!m_bCanMoveFree)
                m_bCanMoveFree = true;

            m_uiWaypointId = 0;
        }

        m_uiMoveNextTimer = 500;
    }

    //used when open portal and spawn mobs in phase
    void DoRaidWhisper(int32 iTextId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    DoScriptText(iTextId, m_creature, i->getSource());
            }
        }
    }

    //"opens" the portal and does the "opening" whisper
    void OpenPortal()
    {
        int32 iTextId = 0;

        //there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        //using a grid search here seem to be more efficient than caching all four guids
        //in instance script and calculate range to each.
        GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature,GO_TWILIGHT_PORTAL,50.0f);

        DoRaidWhisper(WHISPER_OPEN_PORTAL);

        //By using SetRespawnTime() we will actually "spawn" the object with our defined time.
        //Once time is up, portal will disappear again.
        if (pPortal && !pPortal->isSpawned())
            pPortal->SetRespawnTime(m_iPortalRespawnTime);

        //Unclear what are expected to happen if one drake has a portal open already
        //Refresh respawnTime so time again are set to 30secs?
    }

    //Removes each drakes unique debuff from players
    void RemoveDebuff(uint32 uiSpellId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->HasAura(uiSpellId))
                    i->getSource()->RemoveAurasDueToSpell(uiSpellId);
            }
        }
    }

    void RemovePhase()
    {
        RemoveDebuff(SPELL_TWILIGHT_SHIFT_ENTER);
        RemoveDebuff(SPELL_TWILIGHT_SHIFT_DAMAGE);
    }

    void JustDied(Unit* pKiller)
    {
        int32 iTextId = 0;
        uint32 uiSpellId = 0;

        switch(m_creature->GetEntry())
        {
            case NPC_TENEBRON:
                iTextId = SAY_TENEBRON_DEATH;
                uiSpellId = SPELL_POWER_OF_TENEBRON;
                break;
            case NPC_SHADRON:
                iTextId = SAY_SHADRON_DEATH;
                uiSpellId = SPELL_POWER_OF_SHADRON;
                break;
            case NPC_VESPERON:
                iTextId = SAY_VESPERON_DEATH;
                uiSpellId = SPELL_POWER_OF_VESPERON;
                break;
        }

        DoScriptText(iTextId, m_creature);

        RemoveDebuff(uiSpellId);

        if (m_pInstance)
        {
            // not if solo mini-boss fight
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                return;

            // Twilight Revenge to main boss
            if (Unit* pSartharion = Unit::GetUnit((*m_creature), m_pInstance->GetData64(DATA_SARTHARION)))
            {
                if (pSartharion->isAlive())
                    m_creature->CastSpell(pSartharion,SPELL_TWILIGHT_REVENGE,true);
            }
        }
    }
    void DespawnCreatures()
    {
        for(GuidList::iterator itr = Acolytes.begin(); itr != Acolytes.end(); ++itr)
        {
            if (Creature *pCreature = (Creature*)Unit::GetUnit((*m_creature), (*itr)))
            {
                pCreature->ForcedDespawn();
            }
        }

    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bCanMoveFree && m_uiMoveNextTimer)
        {
            if (m_uiMoveNextTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId,
                    m_aDragonCommon[m_uiWaypointId].m_fX, m_aDragonCommon[m_uiWaypointId].m_fY, m_aDragonCommon[m_uiWaypointId].m_fZ);

                debug_log("dummy_dragonAI: %s moving to point %u", m_creature->GetName(), m_uiWaypointId);
                m_uiMoveNextTimer = 0;
            }
            else
                m_uiMoveNextTimer -= uiDiff;
        }
    }
};

/*######
## Mob Tenebron
######*/

struct MANGOS_DLL_DECL mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiHatchTimer;
    int8 m_uiEggsCount;

    void Reset()
    {
        m_uiShadowBreathTimer = 20000;
        m_uiShadowFissureTimer = 5000;
        m_uiHatchTimer = 40000;
        m_uiEggsCount = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_TENEBRON_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_TENEBRON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_TENEBRON_SLAY_1 : SAY_TENEBRON_SLAY_2, m_creature);
    }
    void JustDied(Unit* pKiller)
    {
        dummy_dragonAI::JustDied(pKiller);
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            dummy_dragonAI::DespawnCreatures();
            dummy_dragonAI::RemovePhase();
        }
    }
    void EnterEvadeMode()
    {
        dummy_dragonAI::DespawnCreatures();
        dummy_dragonAI::RemovePhase();
        ScriptedAI::EnterEvadeMode();
    }
    void Summon(uint32 entry, uint32 count, uint32 time = 0)  // whelp or egg
    {
        for(uint32 i = 0; i < count; ++i)
        {
            float angle = float(urand(0, 628)) / 100;
            float destX, destY, destZ;
            m_creature->GetPosition(destX, destY, destZ);

            float dist = float(urand(600, 2500)) / 1000;
            destX += cos(angle)*dist;
            destY += sin(angle)*dist;
            m_creature->UpdateGroundPositionZ(destX, destY, destZ);
            Creature *pSummon = m_creature->SummonCreature(entry, destX, destY, destZ, 0, (time == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN, time);
            if (entry == NPC_TWILIGHT_EGG)
                m_uiEggsCount++;
            if (pSummon)
                dummy_dragonAI::Acolytes.push_back(pSummon->GetGUID());
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->SummonCreature(NPC_TWILIGHT_FISSURE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 8000);

            m_uiShadowFissureTimer = urand(15000, 20000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_TENEBRON_BREATH, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(20000, 25000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        //Hatch eggs...
        if (m_uiHatchTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_HATCH_EGGS);
            if (m_uiEggsCount > 0)
            {
                Summon(NPC_TWILIGHT_WHELP, m_uiEggsCount);
                m_uiEggsCount = 0;
            }
            if (m_uiEggsCount < 0)
                m_uiEggsCount = 0;
            dummy_dragonAI::DoRaidWhisper(WHISPER_HATCH_EGGS);
            m_uiHatchTimer = 20000;
        }else m_uiHatchTimer -= uiDiff;

        //Open portal after 30s
        if (dummy_dragonAI::m_uiPortalTimer <= uiDiff)
        {
            dummy_dragonAI::OpenPortal();
            dummy_dragonAI::m_uiPortalTimer = 30000;
            Summon(NPC_TWILIGHT_EGG, 6, 22500);
            m_uiHatchTimer = 20000;
        }else dummy_dragonAI::m_uiPortalTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_tenebron(Creature* pCreature)
{
    return new mob_tenebronAI(pCreature);
}

/*######
## Mob Shadron
######*/

struct MANGOS_DLL_DECL mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiAcolyteShadronTimer;

    void Reset()
    {
        m_uiShadowBreathTimer = 20000;
        m_uiShadowFissureTimer = 5000;
        m_uiAcolyteShadronTimer = 60000;
        dummy_dragonAI::m_uiPortalTimer = 60000;

        if (m_creature->HasAura(SPELL_TWILIGHT_TORMENT_VESP))
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP);

        if (m_creature->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
            m_creature->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SHADRON_AGGRO,m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_SHADRON);
    }


    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        //"Virtual" imunnity
        if (m_creature->HasAura(SPELL_SPHERE_VISUAL))
        {
            pDoneBy->SendSpellMiss(m_creature, 0, SPELL_MISS_IMMUNE);
            uiDamage = 0;
        }
    }
    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SHADRON_SLAY_1 : SAY_SHADRON_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        dummy_dragonAI::JustDied(pKiller);
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            dummy_dragonAI::DespawnCreatures();
            dummy_dragonAI::RemovePhase();
        }
    }
    void EnterEvadeMode()
    {
        dummy_dragonAI::DespawnCreatures();
        dummy_dragonAI::RemovePhase();
        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->SummonCreature(NPC_TWILIGHT_FISSURE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 8000);
            m_uiShadowFissureTimer = urand(15000, 20000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_SHADRON_BREATH, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(20000, 25000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        //Open portal after 30s
        if (dummy_dragonAI::m_uiPortalTimer <= uiDiff)
        {
            dummy_dragonAI::OpenPortal();
            dummy_dragonAI::m_bHasSpawnedPortal = true;
            dummy_dragonAI::m_uiPortalTimer = 30000;
        }
        else dummy_dragonAI::m_uiPortalTimer -= uiDiff;

        //Summon Acolyte
        if (m_uiAcolyteShadronTimer <= uiDiff)
        {
            // Gift Of Twilight - must be here, acolyte is in another phase
            if (m_pInstance)
            {
                Unit *target = m_creature;
                //if not solo figth, buff main boss, else place debuff on mini-boss. both spells TARGET_SCRIPT
                if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                    target = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SARTHARION));
                if (target)
                    target->CastSpell(target, SPELL_SPHERE_VISUAL, true);
            }
            if (Creature *pCreature = m_creature->SummonCreature(NPC_ACOLYTE_OF_SHADRON, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                dummy_dragonAI::Acolytes.push_back(pCreature->GetGUID());
                pCreature->SetActiveObjectState(true);
            }
            m_uiAcolyteShadronTimer = 60000;
            dummy_dragonAI::DoRaidWhisper(WHISPER_SHADRON_DICIPLE);
            dummy_dragonAI::m_uiAcolytesCount++;
        }else m_uiAcolyteShadronTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadron(Creature* pCreature)
{
    return new mob_shadronAI(pCreature);
}

/*######
## Mob Vesperon
######*/

struct MANGOS_DLL_DECL mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiAcolyteVesperonTimer;

    void Reset()
    {
        m_uiShadowBreathTimer = 20000;
        m_uiShadowFissureTimer = 5000;
        m_uiAcolyteVesperonTimer = 30000;
        dummy_dragonAI::m_uiPortalTimer = 30000; 
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_VESPERON_AGGRO,m_creature);
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_VESPERON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_VESPERON_SLAY_1 : SAY_VESPERON_SLAY_2, m_creature);
    }
    void JustDied(Unit* pKiller)
    {
        dummy_dragonAI::JustDied(pKiller);
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            dummy_dragonAI::DespawnCreatures();
            dummy_dragonAI::RemovePhase();
        }
    }
    void EnterEvadeMode()
    {
        dummy_dragonAI::DespawnCreatures();
        dummy_dragonAI::RemovePhase();
        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {

            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->SummonCreature(NPC_TWILIGHT_FISSURE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
            m_uiShadowFissureTimer = urand(15000, 20000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_VESPERON_BREATH, m_creature);
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(20000, 25000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        //Open portal after 30s
        if (dummy_dragonAI::m_uiPortalTimer <= uiDiff)
        {
            dummy_dragonAI::OpenPortal();
            dummy_dragonAI::m_bHasSpawnedPortal = true;
            dummy_dragonAI::m_uiPortalTimer = 30000;
        } else dummy_dragonAI::m_uiPortalTimer -= uiDiff;

        //Summon Acolyte
        if (m_uiAcolyteVesperonTimer <= uiDiff)
        {
            // Twilight torment - must be here, acolyte is in another phase
            if (m_pInstance)
            {
                Unit *target = m_creature;
                //if not solo figth, buff main boss, else place debuff on mini-boss. both spells TARGET_SCRIPT
                if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                    target = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SARTHARION));
                if (target)
                    target->CastSpell(target, SPELL_TWILIGHT_TORMENT_PLAYERS, true);
            }
            if (Creature *pSummon = m_creature->SummonCreature(NPC_ACOLYTE_OF_VESPERON, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                dummy_dragonAI::Acolytes.push_back(pSummon->GetGUID());

            m_uiAcolyteVesperonTimer = 30000;
            dummy_dragonAI::DoRaidWhisper(WHISPER_VESPERON_DICIPLE);
            dummy_dragonAI::m_uiAcolytesCount++;
        }else m_uiAcolyteVesperonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_vesperon(Creature* pCreature)
{
    return new mob_vesperonAI(pCreature);
}

/*######
## Mob Acolyte of Shadron
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_shadronAI : public ScriptedAI
{
    mob_acolyte_of_shadronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetPhaseMask(16, true);
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            Creature* pDebuffTarget = NULL;
            Creature* pShadron = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SHADRON));

            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                //not solo fight, so main boss has deduff
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SARTHARION));
            }
            else
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SHADRON));

            if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_SPHERE_VISUAL))
            {
                if (pShadron && pShadron->isAlive())
                {
                    if (((dummy_dragonAI*)pShadron->AI())->m_uiAcolytesCount == 1)
                        pShadron->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                    ((dummy_dragonAI*)pShadron->AI())->m_uiAcolytesCount--;
                }else
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_shadron(Creature* pCreature)
{
    return new mob_acolyte_of_shadronAI(pCreature);
}

/*######
## Mob Acolyte of Vesperon
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_vesperonAI : public ScriptedAI
{
    mob_acolyte_of_vesperonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetPhaseMask(16, true);
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        DoCast(m_creature, SPELL_TWILIGHT_TORMENT_PLAYERS, true);
    }

    void JustDied(Unit* pKiller)
    {
        // remove twilight torment on Vesperon
        if (m_pInstance)
        {
            
            Creature* pDebuffTarget = NULL;
            Creature* pVesperon = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_VESPERON));

            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                //not solo fight, so main boss has deduff
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SARTHARION));
            }
            else
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_VESPERON));

            if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_TORMENT_PLAYERS))
            {
                if (pVesperon && pVesperon->isAlive())
                {
                    if (((dummy_dragonAI*)pVesperon->AI())->m_uiAcolytesCount == 1)
                        pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_PLAYERS);
                    ((dummy_dragonAI*)pVesperon->AI())->m_uiAcolytesCount--;
                }else
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_PLAYERS);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_vesperon(Creature* pCreature)
{
    return new mob_acolyte_of_vesperonAI(pCreature);
}

/*######
## Mob Twilight Eggs
######*/

struct MANGOS_DLL_DECL mob_twilight_eggsAI : public ScriptedAI
{
    mob_twilight_eggsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    

    void Reset()
    {
        m_creature->SetPhaseMask(16, true);
    }

    void AttackStart(Unit* pWho) { return; }
    void MoveInLineOfSight(Unit* pWho) { }
    void JustDied(Unit *pWho)
    {
        if (m_pInstance)
        {
            Creature* pTene = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_TENEBRON));

            if (pTene && pTene->isAlive())
                ((mob_tenebronAI*)pTene->AI())->m_uiEggsCount--;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        return;
    }
};

CreatureAI* GetAI_mob_twilight_eggs(Creature* pCreature)
{
    return new mob_twilight_eggsAI(pCreature);
}

/*######
## Mob Twilight Whelps
######*/

struct MANGOS_DLL_DECL mob_twilight_whelpAI : public ScriptedAI
{
    mob_twilight_whelpAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiFadeArmorTimer;
    uint32 switchPhaseTimer;

    void Reset()
    {
        m_uiFadeArmorTimer = 1000;
        switchPhaseTimer = urand(5000, 10000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (switchPhaseTimer <= uiDiff)
            {
                m_creature->SetPhaseMask((m_creature->GetPhaseMask() == 16) ? 1 : 16, true);
                switchPhaseTimer = urand(5000, 10000);
            }else switchPhaseTimer -= uiDiff;
            return;
        }

        // twilight torment
        if (m_uiFadeArmorTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FADE_ARMOR);
            m_uiFadeArmorTimer = urand(5000, 10000);
        }
        else
            m_uiFadeArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_twilight_whelp(Creature* pCreature)
{
    return new mob_twilight_whelpAI(pCreature);
}

/*######
## Mob Flame Tsunami
######*/

struct MANGOS_DLL_DECL mob_flame_tsunamiAI : public ScriptedAI
{
    mob_flame_tsunamiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiTsunamiPoint;
    uint32 m_uiSummonTimer;
    uint32 m_uKnockBackTimer;
    bool isMoving;

    void Reset()
    {
        m_uiSummonTimer = 3000;
        m_uKnockBackTimer = 500;
        isMoving = false;

        DoCast(m_creature, SPELL_FLAME_TSUNAMI, true);
        DoCast(m_creature, SPELL_FLAME_TSUNAMI_DMG_AURA, true);
        m_creature->SetSpeedRate(MOVE_RUN, 3.2f, true);
        m_creature->SetSpeedRate(MOVE_WALK, 3.2f, true);
    }
    void AttackStart(Unit *pWho)
    {
        return;
    }
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        uiDamage = 0;
    }
    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == 1)
            m_creature->ForcedDespawn(1500);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (isMoving)
        {
            if (m_uKnockBackTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_KNOCKOUT, true);
                m_uKnockBackTimer = 500;		
            }else
                m_uKnockBackTimer -= uiDiff;
            return;
         }

        if (m_uiSummonTimer <= uiDiff)
        {
            isMoving = true;
            float x, y, z;
            m_creature->GetPosition(x,y,z);
            if (x < 3280)
                x = 3286;
            else
                x = 3206;

            m_creature->GetMotionMaster()->MovePoint(1, x, y,z);  			
        }else
            m_uiSummonTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_flame_tsunami(Creature* pCreature)
{
    return new mob_flame_tsunamiAI(pCreature);
}
/*######
## Mob Shadow fissure
######*/

struct MANGOS_DLL_DECL mob_twilight_fissureAI : public ScriptedAI
{
    mob_twilight_fissureAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShadowIssure_Timer;

    void AttackStart(){ return; }
    void Reset()
    {
        m_uiShadowIssure_Timer = 5000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //Correct effect not found :(
        DoCast(m_creature, 47651, true);
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiShadowIssure_Timer)
            if (m_uiShadowIssure_Timer < uiDiff)
            {
                Map *map = m_creature->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        uint32 damage = m_bIsRegularMode ? urand(6188, 8812) : urand(9488, 13512);
                        if (i->getSource()->isAlive() && m_creature->GetDistance2d(i->getSource()->GetPositionX(), i->getSource()->GetPositionY()) < 2)
                            m_creature->DealDamage(i->getSource(), damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
                    }
                }
                m_creature->ForcedDespawn();
            }
            else m_uiShadowIssure_Timer -= uiDiff;
    }
};
CreatureAI* GetAI_mob_twilight_fissure(Creature* pCreature)
{
    return new mob_twilight_fissureAI(pCreature);
}

/*######
## go_focusing_iris
######*/

bool GOHello_go_twilight_portal(Player* pPlayer, GameObject* pGo)
{
    // Port to twilight realm...
    if (!pPlayer->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
        pPlayer->CastSpell(pPlayer, SPELL_TWILIGHT_SHIFT_ENTER, true);
    // ...or get the hell out of there!
    else
    {
        pPlayer->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
        pPlayer->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_DAMAGE);
        pPlayer->CastSpell(pPlayer, SPELL_TWILIGHT_RESIDUE, true);
    }
    return true;
}

void AddSC_boss_sartharion()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sartharion";
    newscript->GetAI = &GetAI_boss_sartharion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vesperon";
    newscript->GetAI = &GetAI_mob_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadron";
    newscript->GetAI = &GetAI_mob_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tenebron";
    newscript->GetAI = &GetAI_mob_tenebron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_shadron";
    newscript->GetAI = &GetAI_mob_acolyte_of_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_vesperon";
    newscript->GetAI = &GetAI_mob_acolyte_of_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_eggs";
    newscript->GetAI = &GetAI_mob_twilight_eggs;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_whelp";
    newscript->GetAI = &GetAI_mob_twilight_whelp;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flame_tsunami";
    newscript->GetAI = &GetAI_mob_flame_tsunami;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_fissure";
    newscript->GetAI = &GetAI_mob_twilight_fissure;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_twilight_portal";
    newscript->pGOHello = &GOHello_go_twilight_portal;
    newscript->RegisterSelf();
}