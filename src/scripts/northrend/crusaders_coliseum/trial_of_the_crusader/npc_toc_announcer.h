/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ANNOUNCER_H
#define DEF_ANNOUNCER_H

#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature);

    ScriptedInstance* m_pInstance;
    Difficulty        m_dDifficulty;
    bool              isHeroic;
    bool              is10Man;
    int32             currentEncounter;
    uint16            encounterStage;
    uint32            customValue;
    Creature*         encounterCreature;
    Creature*         encounterCreature2;

    void Reset();
    void AttackStart(Unit* /*who*/);
    void ChooseEvent(uint8 encounterId, Player* chooser = NULL);
    void DataSet(uint32 type, uint32 data);
    void MovementInform(uint32 uiType, uint32 uiPointId);
    Creature* DoSpawnTocBoss(uint32 id, Coords coord, float ori, bool update_z = true);
    void SummonToCBoss(uint32 id, uint32 id2 = 0, uint32 dooropen = 500);
    void UpdateAI(const uint32 /*diff*/);
    void DeleteCreaturesAndRemoveAuras();
};

bool GossipHello_npc_toc_announcer(Player* player, Creature* creature);
bool GossipSelect_npc_toc_announcer(Player* player, Creature* creature, uint32 /*sender*/, uint32 action);
CreatureAI* GetAI_npc_toc_announcer(Creature* creature);
#endif
