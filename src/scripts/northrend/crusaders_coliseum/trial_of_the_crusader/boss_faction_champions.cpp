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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

/* ScriptData
SDName: faction_champions
SD%Complete: 60%
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum RoleFCH
{
    ROLE_HEALER             = 0,
    ROLE_RANGED             = 1,
    ROLE_MELEE              = 2
};

enum HealPower
{
    HEAL_NOHEAL             = 0,
    HEAL_MINOR              = 1,
    HEAL_MIDDLE             = 2,
    HEAL_MAJOR              = 3,
    HEAL_LIFESAVING         = 4,
    HEAL_COUNT              = 5
};

enum Timers
{
    // 1-100 reserved for mutual
    TIMER_HEAL      = 1, // 1-5
    TIMER_DISPEL    = 6
};

struct FactionedChampionAI : public ScriptedAI
{
    FactionedChampionAI(Creature* pCreature, RoleFCH pRole, Champion pChamp) : ScriptedAI(pCreature),
    m_role(pRole), m_champType(pChamp)
    {
        m_pInstance     = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty   = pCreature->GetMap()->GetDifficulty();
        m_faction       = FChampIDs[m_champType][FACTION_ALLIANCE] == pCreature->GetEntry() ? FACTION_ALLIANCE : FACTION_HORDE;

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    ScriptedInstance*   m_pInstance;
    Difficulty          m_dDifficulty;

    RoleFCH             m_role;
    FactionFCH          m_faction;
    Champion            m_champType;

    CreatureList        m_cAliveChampions;

    bool                m_bHasMagicalCC;

    void Reset()
    {
        m_bHasMagicalCC = false;
    }

    FactionedChampionAI* GetFactionedAI(Creature* crt)
    {
        return crt && IsEntryChampion(crt->GetEntry()) ? (FactionedChampionAI*)crt->AI() : NULL;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (pSpell->Effect[i] != SPELL_EFFECT_APPLY_AURA)
                continue;

            if (pSpell->EffectApplyAuraName[i] != SPELL_AURA_MOD_STUN &&
                pSpell->EffectApplyAuraName[i] != SPELL_AURA_MOD_FEAR &&
                pSpell->EffectApplyAuraName[i] != SPELL_AURA_MOD_ROOT &&
                pSpell->EffectApplyAuraName[i] != SPELL_AURA_MOD_PACIFY &&
                pSpell->EffectApplyAuraName[i] != SPELL_AURA_MOD_CONFUSE)
                continue;

            if (pSpell->Dispel == DISPEL_MAGIC)
                m_bHasMagicalCC = true;
        }
    }

    void RefreshListOfAliveChampions()
    {
        m_cAliveChampions.clear();
         for(uint8 i = 0; i < CHAMPION_ALL_COUNT; ++i)
            if (Creature* pCrt = GetClosestCreatureWithEntry(m_creature, FChampIDs[i][m_faction], DEFAULT_VISIBILITY_INSTANCE))
                if (pCrt->isAlive())
                    m_cAliveChampions.push_back(pCrt);
    }

    PlrList GetPlayersInInstance(bool alive_only)
    {
        PlrList plr_list;
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

        if (!PlayerList.isEmpty())
            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (!alive_only || i->getSource()->isAlive())
                    plr_list.push_back(i->getSource());

        return plr_list;
    }

    bool IsEntryChampion(uint32 entry)
    {
        if (!entry)
            return false;

        for(uint8 i = 0; i < CHAMPION_ALL_COUNT; ++i)
            if (FChampIDs[i][m_faction] == entry)
                return true;

        return false;
    }

    CreatureList GetAllFCh()
    {
        CreatureList list;
        for(uint8 i = 0; i < CHAMPION_ALL_COUNT; ++i)
            GetCreatureListWithEntryInGrid(list, m_creature, FChampIDs[i][m_faction], DEFAULT_VISIBILITY_INSTANCE);

        return list;
    }

    Creature* GetChampionWithDispellableCC()
    {
        for(CreatureList::iterator itr = m_cAliveChampions.begin(); itr != m_cAliveChampions.end(); ++itr)
            if (FactionedChampionAI* pTemp = GetFactionedAI(*itr))
                if (pTemp->m_bHasMagicalCC)
                    return *itr;

        return NULL;
    }


    void UseTrinket()
    {
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_STUN);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_FEAR);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_PACIFY);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_CONFUSE);
        //DoCast(m_creature, SPELL_PVP_TRINKET);
    }
};

typedef std::list<FactionedChampionAI*> FChampList;

//////////////////
////  HEALERS ////
//////////////////

//resto druid, holy paladin, disci priest, resto shaman
//4 scripts

struct factioned_healerAI : public FactionedChampionAI
{
    factioned_healerAI(Creature* pCreature, Champion pChamp) : FactionedChampionAI(pCreature, ROLE_HEALER, pChamp)
    {
    }

    //SpellTimer m_heals[HEAL_COUNT];

    void Reset()
    {
        FactionedChampionAI::Reset();
    }

    Creature* SelectChampionWithLowestHp()
    {
        Creature* lowestCrt = NULL;
        for(CreatureList::iterator itr = m_cAliveChampions.begin(); itr != m_cAliveChampions.end(); ++itr)
            if (*itr)
                if (!lowestCrt || (*itr)->GetHealthPercent() < lowestCrt->GetHealthPercent())
                    lowestCrt = *itr;

        return lowestCrt;
    }

    HealPower GetAppropriateHealPowerForTarget(Creature* target)
    {
        if (target->GetHealthPercent() > 90)
            return HEAL_NOHEAL;

        else if (target->GetHealthPercent() > 80)
            return HEAL_MINOR;

        else if (target->GetHealthPercent() > 55)
            return HEAL_MIDDLE;

        else if (target->GetHealthPercent() > 30)
            return HEAL_MAJOR;

        else
            return HEAL_LIFESAVING;
    }

    HealPower SelectActualHealPower(HealPower appropHealPower)
    {
        for (uint8 healpower = appropHealPower; healpower > 0; --healpower)
        {
            SpellTimer* heal = m_TimerMgr->GetTimer(TIMER_HEAL+healpower);
            if (heal && heal->IsReady())
                return HealPower(healpower);
        }

        return HEAL_NOHEAL;
    }

    bool DoHeal()
    {
        Creature* target = SelectChampionWithLowestHp();
        HealPower appropHealPower = GetAppropriateHealPowerForTarget(target);
        if (appropHealPower == HEAL_NOHEAL)
            return false;

        HealPower castedHP = SelectActualHealPower(appropHealPower);
        if (castedHP == HEAL_NOHEAL)
            return false;

        m_TimerMgr->TimerFinished(TIMER_HEAL+castedHP, target);
        return true;
    }

    uint32 timer(HealPower healpower)
    {
        return TIMER_HEAL+healpower;
    }

};

enum Resto_Druid
{
    RD_REJUVENATION     = 66065,
    RD_LIFEBLOOM        = 66093,
    RD_REGROWTH         = 66067,
    RD_NOURISH          = 66066,

    RD_NATURES_GRASP    = 66071,
    RD_BARSKIN          = 65860,
    RD_THORNS           = 66068,
    RD_TRANQUILITY      = 66086
};

struct champ_rdruidAI : public factioned_healerAI
{
    champ_rdruidAI(Creature* pCreature) : factioned_healerAI(pCreature, CHAMPION_R_DRUID)
    {
        Reset();
    }

    void Reset()
    {
        factioned_healerAI::Reset();

        // healer timers
        m_TimerMgr->AddTimer(timer(HEAL_MINOR), RD_REJUVENATION, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MIDDLE), RD_LIFEBLOOM, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MAJOR), RD_REGROWTH, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_LIFESAVING), RD_NOURISH, 0, 2000);

        m_TimerMgr->AddTimer(RD_TRANQUILITY, RD_TRANQUILITY, 15000, 600000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);
        m_TimerMgr->AddTimer(RD_NATURES_GRASP, RD_NATURES_GRASP, 2000, 60000, UNIT_SELECT_SELF, CAST_TYPE_QUEUE);
        m_TimerMgr->AddTimer(RD_THORNS, RD_THORNS, 0, 2000);
        m_TimerMgr->AddTimer(RD_BARSKIN, RD_BARSKIN, 2000, 60000, UNIT_SELECT_SELF, CAST_TYPE_QUEUE);
    }

    bool CanCastTranquility()
    {
        uint8 counts[2] = {0, 0};
        for(CreatureList::iterator itr = m_cAliveChampions.begin(); itr != m_cAliveChampions.end(); ++itr)
        {
            bool lowHp = (*itr)->GetHealthPercent() < 50;
            ++counts[int(lowHp)];
        }

        return counts[1] > counts[0];
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Tranquility
        if (CanCastTranquility())
            m_TimerMgr->TimerFinished(RD_TRANQUILITY);

        // cast heal if it suits situation, if does, return function
        if (DoHeal())
            return;
    }
};

enum Holy_Paladin
{
    HP_HOLY_LIGHT       = 66112,
    HP_HOLY_SHOCK       = 66114,
    HP_FLASH_OF_LIGHT   = 66113,
    HP_HAND_OF_PROTECTION=66009,

    HP_CLEANSE          = 66116,
    HP_DIVINE_SHIELD    = 66010,
    HP_HAMMER_OF_JUSTICE= 66613,
    HP_HAND_OF_FREEDOM  = 66615
};

struct champ_hpalaAI : public factioned_healerAI
{
    champ_hpalaAI(Creature* pCreature) : factioned_healerAI(pCreature, CHAMPION_H_PALADIN)
    {
        Reset();
    }

    void Reset()
    {
        factioned_healerAI::Reset();

        m_TimerMgr->AddTimer(timer(HEAL_MINOR), HP_HOLY_LIGHT, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MIDDLE), HP_HOLY_SHOCK, 0, 6000);
        m_TimerMgr->AddTimer(timer(HEAL_MAJOR), HP_FLASH_OF_LIGHT, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_LIFESAVING), HP_HAND_OF_PROTECTION, 2000, 300000);

        m_TimerMgr->AddTimer(TIMER_DISPEL, HP_CLEANSE, 15000, 2000);
        m_TimerMgr->AddTimer(HP_DIVINE_SHIELD, HP_DIVINE_SHIELD, 2000, 300000, UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(HP_HAMMER_OF_JUSTICE, HP_HAMMER_OF_JUSTICE, 0, 40000);
        m_TimerMgr->AddTimer(HP_HAND_OF_FREEDOM, HP_HAND_OF_FREEDOM, 2000, 25000);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // cast heal if it suits situation, if does, return function
        if (DoHeal())
            return;
    }
};

enum Disco_Priest
{
    DP_RENEW            = 66177,
    DP_FLASH_HEAL       = 66104,
    //DP_PENANCE          = 66098,
    DP_PENANCE          = 66097,
    DP_POWER_WORD_SHIELD= 66099,

    DP_DISPEL_MAGIC     = 65546,
    DP_MANA_BURN        = 66100,
    DP_PSYCHIC_SCREAM   = 65543

};

struct champ_dpriestAI : public factioned_healerAI
{
    champ_dpriestAI(Creature* pCreature) : factioned_healerAI(pCreature, CHAMPION_D_PRIEST)
    {
        Reset();
    }

    void Reset()
    {
        factioned_healerAI::Reset();

        m_TimerMgr->AddTimer(timer(HEAL_MINOR), DP_RENEW, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MIDDLE), DP_FLASH_HEAL, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MAJOR), DP_PENANCE, 0, 10000);
        m_TimerMgr->AddTimer(timer(HEAL_LIFESAVING), DP_POWER_WORD_SHIELD, 0000, 15000);

        m_TimerMgr->AddTimer(TIMER_DISPEL, DP_DISPEL_MAGIC, 2000, 2000);
        m_TimerMgr->AddTimer(DP_MANA_BURN, DP_MANA_BURN, 2000, 2000);
        m_TimerMgr->AddTimer(DP_PSYCHIC_SCREAM, DP_PSYCHIC_SCREAM, 2000, 30000, UNIT_SELECT_SELF);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // cast heal if it suits situation, if does, return function
        if (DoHeal())
            return;
    }
};

enum Resto_Shaman
{
    RS_EARTH_SHIELD     = 66063,
    RS_LESSER_HEALING_W = 66055,
    RS_RIPTIDE          = 66053,

    RS_CLEANSE_SPIRIT   = 66056,

    RS_EARH_SHOCK       = 65973,
    RS_HEROISM          = 65983,
    RS_BLOODLUST        = 65980,
    RS_HEX              = 66054
};

struct champ_rshamAI : public factioned_healerAI
{
    champ_rshamAI(Creature* pCreature) : factioned_healerAI(pCreature, CHAMPION_R_SHAMAN)
    {
        Reset();
    }

    void Reset()
    {
        factioned_healerAI::Reset();

        m_TimerMgr->AddTimer(timer(HEAL_MINOR), RS_EARTH_SHIELD, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MIDDLE), RS_LESSER_HEALING_W, 0, 2000);
        m_TimerMgr->AddTimer(timer(HEAL_MAJOR), RS_RIPTIDE, 0, 6000);

        m_TimerMgr->AddTimer(TIMER_DISPEL, RS_CLEANSE_SPIRIT, 2000, 2000);
        m_TimerMgr->AddTimer(RS_EARH_SHOCK, RS_EARH_SHOCK, 2000, 6000);
        m_TimerMgr->AddTimer(RS_HEROISM, m_creature->GetEntry() == 34444 ? RS_BLOODLUST : RS_HEROISM, 0, 300000, UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(RS_HEX, RS_HEX, 2000, 45000);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // cast heal if it suits situation, if does, return function
        if (DoHeal())
            return;
    }
};

//////////////////
/////  MELEE /////
//////////////////

//frost dk, retri paladin, sub rogue, enha shaman, arms warr
//5 scripts

struct factioned_meleeAI : public FactionedChampionAI
{
    factioned_meleeAI(Creature* pCreature, Champion pChamp) : FactionedChampionAI(pCreature, ROLE_HEALER, pChamp)
    {
    }
};

enum Death_Knight
{
    DK_DEATH_COIL           = 66019,
    DK_DEATH_GRIP           = 66017,
    DK_FROST_STRIKE         = 66047,
    DK_CHAINS_OF_ICE        = 66020,
    DK_ICEBOUND_FORTITUDE   = 66023,
    DK_ICY_TOUCH            = 66021,
    DK_STRANGULATE          = 66018,
};



//////////////////
////  RANGED  ////
//////////////////


//balanc druid, surv hunta, arcane mage, shadow priest, affi lock
//5 scripts



struct factioned_rangedAI : public FactionedChampionAI
{
    factioned_rangedAI(Creature* pCreature, Champion pChamp) : FactionedChampionAI(pCreature, ROLE_HEALER, pChamp)
    {
    }
};

void AddSC_boss_faction_champions()
{
/*   Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_toc_druid";
    newscript->GetAI = &GetAI_mob_toc_druid;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shaman";
    newscript->GetAI = &GetAI_mob_toc_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_paladin";
    newscript->GetAI = &GetAI_mob_toc_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_priest";
    newscript->GetAI = &GetAI_mob_toc_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shadow_priest";
    newscript->GetAI = &GetAI_mob_toc_shadow_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_mage";
    newscript->GetAI = &GetAI_mob_toc_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warlock";
    newscript->GetAI = &GetAI_mob_toc_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_hunter";
    newscript->GetAI = &GetAI_mob_toc_hunter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_boomkin";
    newscript->GetAI = &GetAI_mob_toc_boomkin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warrior";
    newscript->GetAI = &GetAI_mob_toc_warrior;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_dk";
    newscript->GetAI = &GetAI_mob_toc_dk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_rogue";
    newscript->GetAI = &GetAI_mob_toc_rogue;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_enh_shaman";
    newscript->GetAI = &GetAI_mob_toc_enh_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_retro_paladin";
    newscript->GetAI = &GetAI_mob_toc_retro_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_warlock";
    newscript->GetAI = &GetAI_mob_toc_pet_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_hunter";
    newscript->GetAI = &GetAI_mob_toc_pet_hunter;
    newscript->RegisterSelf();*/
}
