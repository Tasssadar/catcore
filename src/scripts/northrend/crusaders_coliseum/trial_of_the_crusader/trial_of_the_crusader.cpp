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
SDName: Trial Of the crusader
SD%Complete: nevim%
SDComment: Spammca
SDCategory: trial_of_the_crusader
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

bool GossipHello_npc_toc_argentmage(Player* pPlayer, Creature* pCreature)
{
    char const* _message = "Please port me up to the surface!";
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc_argentmage(Player* pPlayer, Creature* , uint32 , uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        pPlayer->TeleportTo(WorldLocation(pPlayer->GetMapId(), SpawnLoc[LOC_CENTER], 0));
    return true;
}

void AddSC_trial_of_the_crusader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc_argentmage";
    NewScript->pGossipHello = &GossipHello_npc_toc_argentmage;
    NewScript->pGossipSelect = &GossipSelect_npc_toc_argentmage;
    NewScript->RegisterSelf();
}
