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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Emalon
SD%Complete: 90%
SDComment: 
SDAuthor: Danstahr
SDCategory: vault_of_archavon
EndScriptData */


#include "precompiled.h"
#include "TemporarySummon.h"

#define C_MINION_R          34049
#define C_MINION_H          33998

#define S_BERSERK           26662
#define S_LIGHTNINGNOVA_R   64216
#define S_LIGHTNINGNOVA_H   65279
#define S_CHAINLIGHTNING_R  64213
#define S_CHAINLIGHTNING_H  64215
#define S_OVERCHARGED       64379

#define SAY_OVERCHARGE   -1624010
#define SAY_SUMMONED     -1624011

const float MinionPos[4][3]=
{
    {-204.98f, -281.20f, 91.5790f},
    {-205.06f, -296.94f, 91.5764f},
    {-232.59f, -281.03f, 91.5679f},
    {-232.76f, -296.84f, 91.6013f}
};

struct MANGOS_DLL_DECL boss_emalonAI : public ScriptedAI
{
    boss_emalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        RegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 EnrageTimer;
    uint16 OverchargeTimer;
    uint16 ChainLightningTimer;
    uint16 MinionsCheckTimer;
    uint16 LightningNovaTimer;
    uint64 MinionsGUID[4];
    bool SpawnedOOC;
    bool RegularMode;

    void Reset()
    {
        EnrageTimer=360000;
        LightningNovaTimer=urand(20000,25000);
        OverchargeTimer=45000;
        ChainLightningTimer=urand(7000,8000);
        for (uint8 i=0;i<4;i++)
            MinionsGUID[i]=0;
        MinionsCheckTimer=8000;
        SpawnedOOC=false;
    }

    void CheckForMinionsCombat()
    {
        for (uint8 i=0;i<4;i++)
        {
            if (!MinionsGUID[i])
            {
                float x,y,z;
                m_creature->GetPosition(x,y,z);
                if (Creature* pCreature = m_creature->SummonCreature(RegularMode ? C_MINION_R : C_MINION_H,x,y,z,m_creature->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN,3000))
                {
                    MinionsGUID[i]=pCreature->GetGUID();
                    pCreature->SetOwnerGUID(m_creature->GetGUID());
                    Map* pMap = m_creature->GetMap();
                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();
                        if (!PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                DoScriptText(SAY_SUMMONED,m_creature,i->getSource());
                        }   
                    }
                }
            }
        }
    }

    void RespawnMinions()
    {
        m_creature->AI()->JustDied(NULL);
        for (uint8 i=0;i<4;i++)
            if (Creature* pCreature = m_creature->SummonCreature(RegularMode ? C_MINION_R : C_MINION_H,MinionPos[i][0],MinionPos[i][1],MinionPos[i][2],m_creature->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN,3000))
            {
                MinionsGUID[i]=pCreature->GetGUID();
                pCreature->SetOwnerGUID(m_creature->GetGUID());
            }
        SpawnedOOC=true;
    }

    void MinionDead(uint64 MinGUID)
    {
        for (uint8 i=0;i<4;i++)
            if (MinGUID==MinionsGUID[i])
            {
                MinionsGUID[i]=0;
                break;
            }
    }

    void Aggro(Unit* who)
    {
        m_creature->SetInCombatWithZone();
        for (uint8 i = 0; i < 4; i++)
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(MinionsGUID[i]))
                pCreature->AI()->AttackStart(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!SpawnedOOC)
            RespawnMinions();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if ((m_creature->getVictim()->GetEntry()==C_MINION_R)||(m_creature->getVictim()->GetEntry()==C_MINION_H))
            m_creature->AI()->EnterEvadeMode();

        if (EnrageTimer<diff)
        {
            DoCast(m_creature,S_BERSERK);
            EnrageTimer=360000;
        }
        else EnrageTimer-=diff;

        if (ChainLightningTimer<diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                RegularMode ? DoCast(target,S_CHAINLIGHTNING_R) : DoCast(target,S_CHAINLIGHTNING_H);
            ChainLightningTimer=urand(15000,20000);
        }
        else ChainLightningTimer-=diff;

        if (LightningNovaTimer<diff)
        {
            RegularMode ? DoCast(m_creature,S_LIGHTNINGNOVA_R) : DoCast(m_creature,S_LIGHTNINGNOVA_H);
            LightningNovaTimer=urand(30000,40000);
        }
        else LightningNovaTimer-=diff;

        if (OverchargeTimer<diff)
        {
            uint8 rnd=rand()%4;
            if (MinionsGUID[rnd])
            {
                if (Creature* pCreature = m_creature->GetMap()->GetCreature(MinionsGUID[rnd]))
                {
                    //a little hack since spell 64218 is not working well
                    pCreature->CastSpell(pCreature,S_OVERCHARGED,false);
                    pCreature->SetHealth(pCreature->GetMaxHealth());
                    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X,1);
                    Map* pMap = m_creature->GetMap();
                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();
                        if (!PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                DoScriptText(SAY_OVERCHARGE,m_creature,i->getSource());
                        }   
                    }
                }
                OverchargeTimer=45000;
            }
        }
        else OverchargeTimer-=diff;

        if (MinionsCheckTimer<diff)
        {
            CheckForMinionsCombat();
            MinionsCheckTimer=8000;
        }
        else MinionsCheckTimer-=diff;

        DoMeleeAttackIfReady();

    }

    void EnterEvadeMode()
    {
        m_creature->AI()->JustDied(NULL);
        ScriptedAI::EnterEvadeMode();
    }


    void JustDied(Unit* who)
    {
        for (uint8 i = 0; i < 4; i++)
        {
            if (Creature* pCreature = m_creature->GetMap()->GetCreature(MinionsGUID[i]))
                ((TemporarySummon*)pCreature)->UnSummon();
            MinionsGUID[i]=0;
        }
    }
};

#define S_SHOCK             64363
#define S_OVERCHARGE        64217
#define S_OVERCHARGED_BLAST 64219

#define C_EMALON            33993

struct MANGOS_DLL_DECL mob_tempest_minionAI : public ScriptedAI
{
    mob_tempest_minionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint16 ShockTimer;
    uint16 BoomCheckTimer;

    bool ShouldBeDead;

    void Reset()
    {
        ShouldBeDead=false;
        BoomCheckTimer=10000;
        ShockTimer=urand(2000,10000);
    }

    void Aggro(Unit* who)
    {
        if (Creature* pEmalon = GetClosestCreatureWithEntry(m_creature,C_EMALON,100.0f))
        {
            pEmalon->AI()->AttackStart(who);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (ShouldBeDead)
        {
            m_creature->DealDamage(m_creature,m_creature->GetHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        
        if (ShockTimer<diff)
        {
            DoCast(m_creature->getVictim(),S_SHOCK);
            ShockTimer=urand(6000,10000);
        }
        else ShockTimer-=diff;

        if (BoomCheckTimer<diff)
        {
            Aura* pAura = m_creature->GetAura(S_OVERCHARGE,EFFECT_INDEX_0);
            if (pAura)
            {
                if ((pAura->GetStackAmount())>=10)
                {
                    DoCast(m_creature,S_OVERCHARGED_BLAST);
                    ShouldBeDead=true;
                }
            }
            BoomCheckTimer=1000;
        }
        else BoomCheckTimer-=diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* who)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_creature->GetOwnerGUID()))
            if (pCreature->GetEntry()==C_EMALON)
                ((boss_emalonAI*)pCreature->AI())->MinionDead(m_creature->GetGUID());
    }
};

CreatureAI* GetAI_boss_emalon(Creature* pCreature)
{
    return new boss_emalonAI(pCreature);
}

CreatureAI* GetAI_mob_tempest_minion(Creature* pCreature)
{
    return new mob_tempest_minionAI(pCreature);
}

void AddSC_boss_emalon()
{   
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_emalon";
    newscript->GetAI = &GetAI_boss_emalon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tempest_minion";
    newscript->GetAI = &GetAI_mob_tempest_minion;
    newscript->RegisterSelf();
}