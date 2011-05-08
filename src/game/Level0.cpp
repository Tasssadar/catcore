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
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "AccountMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "revision_nr.h"
#include "Util.h"
#include "GameEventMgr.h"

bool ChatHandler::HandleHelpCommand(const char* args)
{
    char* cmd = strtok((char*)args, " ");
    if (!cmd)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), cmd))
            SendSysMessage(LANG_NO_HELP_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(const char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(const char* /*args*/)
{
    AccountTypes gmlevel = GetAccessLevel();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(const char* /*args*/)
{
    Player *chr = m_session->GetPlayer();

    if (chr->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (chr->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }
    
    if (m_session->GetPlayer()->GetMapId() == 609)
    {
        SendSysMessage("You cannot use this command in this location!");
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr,7355,false);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(const char* /*args*/)
{
    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    //uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    //uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string str = secsToTimeString(sWorld.GetUptime());
    char const* valhalla_rev = REVISION_VP;
    char const* valhalla_rev_date = REVISION_VP_DATE;

    PSendSysMessage("CatCore [%s][%s] - MaNGOS modified for Valhalla Server", valhalla_rev, valhalla_rev_date);
    
    if (GetAccessLevel() > SEC_PLAYER)
    {
        //Dont use mangos revision anymore since we backport only few commits
        char const* full;
        if (m_session)
            full = _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_NR,"|cffffffff|Hurl:" REVISION_ID "|h" REVISION_ID "|h|r");
        else
            full = _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_NR,REVISION_ID);

        SendSysMessage(full);
    
        PSendSysMessage(LANG_USING_SCRIPT_LIB,sWorld.GetScriptsVersion());
        PSendSysMessage(LANG_USING_WORLD_DB,sWorld.GetDBVersion());
        PSendSysMessage(LANG_USING_EVENT_AI,sWorld.GetCreatureEventAIVersion());
    }
    SendSysMessage("Changelog: http://valhalla-team.com/web/changelog.php");
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum);
    PSendSysMessage(LANG_UPTIME, str.c_str());
    PSendSysMessage("World diff time: %u", sWorld.GetDiffTime());
    PSendSysMessage("Total players joined in Dungeon Finder: %u", sLfgMgr.GetTotalPlayers());
    if (sWorld.IsShutdowning())
    {
        const char *type = (sWorld.GetShutdownMask() & SHUTDOWN_MASK_RESTART) ? "Restart" : "Shutdown";
        uint32 shutdownTimer = sWorld.GetShutdownTimer();
        if (shutdownTimer > 60*60) //Hours
        {
            uint8 hours = shutdownTimer / (60*60);
            uint8 mins = (shutdownTimer - hours*60*60) / 60;
            uint8 secs = (shutdownTimer - hours*60*60 - mins*60);
            PSendSysMessage("[SERVER] %s in %u hours, %u minutes and %u seconds", type, hours, mins, secs);
        }
        else if (shutdownTimer > 60) // Minutes
        {
            uint8 mins = shutdownTimer / 60;
            uint8 secs = (shutdownTimer - mins*60);
            PSendSysMessage("[SERVER] %s in %u minutes and %u seconds", type,  mins, secs);
        }
        else //Only seconds
            PSendSysMessage("[SERVER] %s in %u seconds", type, shutdownTimer);
    }
    return true;
}

bool ChatHandler::HandleDismountCommand(const char* /*args*/)
{
    //If player is not mounted, so go out :)
    if (!m_session->GetPlayer( )->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer( )->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->Unmount();
    m_session->GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(const char* /*args*/)
{
    Player *player=m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if (GetAccessLevel() > SEC_PLAYER)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_UINT32_INTERVAL_SAVE);
    if (save_interval==0 || (save_interval > 20*IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20*IN_MILLISECONDS))
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandleGMListIngameCommand(const char* /*args*/)
{
    std::list< std::pair<std::string, bool> > names;

    {
        HashMapHolder<Player>::ReadGuard g(HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType &m = sObjectAccessor.GetPlayers();
        for(HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
            if ((itr->second->isGameMaster() || (itr_sec > SEC_PLAYER && itr_sec <= (AccountTypes)sWorld.getConfig(CONFIG_UINT32_GM_LEVEL_IN_GM_LIST))) &&
                (!m_session || itr->second->IsVisibleGloballyFor(m_session->GetPlayer())))
                names.push_back(std::make_pair<std::string, bool>(GetNameLink(itr->second), itr->second->isAcceptWhispers()));
        }
    }

    if (!names.empty())
    {
        SendSysMessage(LANG_GMS_ON_SRV);

        char const* accepts = GetMangosString(LANG_GM_ACCEPTS_WHISPER);
        char const* not_accept = GetMangosString(LANG_GM_NO_WHISPER);
        for(std::list<std::pair< std::string, bool> >::const_iterator iter = names.begin(); iter != names.end(); ++iter)
            PSendSysMessage("%s - %s", iter->first.c_str(), iter->second ? accepts : not_accept);
    }
    else
        SendSysMessage(LANG_GMS_NOT_LOGGED);

    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(const char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage (LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage (true);
        return false;
    }

    if (!*args)
        return false;

    char *old_pass = strtok ((char*)args, " ");
    char *new_pass = strtok (NULL, " ");
    char *new_pass_c  = strtok (NULL, " ");

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (password_new != password_new_c)
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    if (!sAccountMgr.CheckPassword (GetAccountId(), password_old))
    {
        SendSysMessage (LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(GetAccountId(), password_new);

    switch(result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandleAccountLockCommand(const char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage (LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage (true);
        return false;
    }

    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    std::string argstr = (char*)args;
    if (argstr == "on")
    {
        LoginDatabase.PExecute( "UPDATE account SET locked = '1' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
        return true;
    }

    if (argstr == "off")
    {
        LoginDatabase.PExecute( "UPDATE account SET locked = '0' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(const char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}


bool ChatHandler::HandleLitakCommand(const char* args)
{
    std::string argstr = (char*)args;
    Player *player=m_session->GetPlayer();

    if (player->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }
    if (player->isInFlight())
        player->Unmount();
    player->clearUnitState(UNIT_STAT_IN_FLIGHT);
    player->GetMotionMaster()->Clear(false, true);
    player->GetMotionMaster()->MoveIdle();

    if (argstr == "textura")
    {
        if (!player->m_taxi.empty())
        {
            TaxiNodesEntry const* curSrcNode = sTaxiNodesStore.LookupEntry(player->m_taxi.GetTaxiSource());
            player->m_taxi.ClearTaxiDestinations();
            if (curSrcNode)
                player->TeleportTo(curSrcNode->map_id, curSrcNode->x, curSrcNode->y, curSrcNode->z, 0);
            else
                player->MonsterMove(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0);
        }
        else
        {
            player->m_taxi.ClearTaxiDestinations();
            player->MonsterMove(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0);
        }
    }
    player->m_taxi.ClearTaxiDestinations();
    
    player->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT);
    player->StopMoving();
    player->SendInitialActionButtons();
    return true;
}

bool ChatHandler::HandleTestCommand(const char * /*args*/)
{
    return true;
}

bool ChatHandler::HandleArenaInfoCommand(const char * /*args*/)
{
    std::ostringstream ss;
    bool twos = sGameEventMgr.IsActiveEventForArena(ARENA_TYPE_2v2);
    bool threes = sGameEventMgr.IsActiveEventForArena(ARENA_TYPE_3v3);
    if (!twos && !threes)
    {
        ss << "No arena brackets are active right now";
    }
    else
    {
        ss << "Currently active arena brackets are: ";
        if (twos)
            ss << "2v2 ";
        if (threes)
            ss << "3v3 ";
    }

    SendSysMessage( ss.str().c_str() );

    return true;
}
