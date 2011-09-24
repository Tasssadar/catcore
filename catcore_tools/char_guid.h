#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sstream>
#include <map>
#include <stdarg.h>

#define OFFSET_CHARACTERS 1000 // First guid will be 1
#define OFFSET_ITEMS 1000
#define OFFSET_PETS 1000
#define OFFSET_GUILD 1000
#define OFFSET_MAIL 1000
#define OFFSET_ACC 1000

#define MAX_STEPS 6
#define REPEATING 1

char *server = "localhost";
char *user = "user";
char *password = ""; /* set me first */
char *database_char = "characters";
char *database_acc = "auth";

using namespace std;
typedef unsigned char uint8;

FILE *log_file;
FILE *state_file;
MYSQL *conn_char = NULL;
MYSQL *conn_acc = NULL;
uint8 state = 0;
bool changeGuids(char *main_table, char *main_col, char *tables[][2], uint tables_count, uint offset, MYSQL *con,
                 MYSQL *con_acc = NULL, char *tables_acc[][2] = NULL, uint tables_count_acc = 0);

static const uint8 char_tables_count = 23;
char *tab_char[][2] =
{
    {"character_account_data", "guid"}, //1
    {"character_action", "guid"}, //2
    {"character_achievement", "guid"},//3
    {"character_achievement_progress", "guid"}, //4
    {"character_glyphs", "guid"}, //5
    {"character_homebind", "guid"}, //6
    {"character_inventory", "guid"}, //7
    {"character_queststatus", "guid"}, //8
    {"character_queststatus_rewarded", "guid"}, //9
    {"character_reputation", "guid"}, //10
    {"character_skills", "guid"}, //11
    {"character_social", "guid"}, //12
    {"character_social", "friend"}, //13
    {"character_spell", "guid"}, //14
    {"mail", "sender"}, //15
    {"mail", "receiver"}, //16
    {"mail_items", "receiver"}, //17
    {"character_pet", "owner"}, //18
    {"guild", "leaderguid"}, //19
    {"guild_member", "guid"}, // 20
    {"item_instance", "owner_guid"}, //21
    {"item_instance", "creatorGuid"}, //22
    {"item_instance", "giftCreatorGuid"} //23
};

static const uint8 guild_tables_count = 5;
char *tab_guild[][2] =
{
    {"guild_bank_item", "guildid"}, //1
    {"guild_bank_right", "guildid"}, //2
    {"guild_bank_tab", "guildid"},//3
    {"guild_member", "guildid"}, //4
    {"guild_rank", "guildid"}, //5
};

static const uint8 item_tables_count = 4;
char *tab_item[][2] =
{
    {"mail_items", "item_guid"}, //1
    {"character_inventory", "bag"}, //2
    {"character_inventory", "item"}, //3
    {"guild_bank_item", "item_guid"},//4
};

static const uint8 pet_tables_count = 1;
char *tab_pet[][2] =
{
    {"pet_spell", "guid"}, //1
};

static const uint8 mail_tables_count = 1;
char *tab_mail[][2] =
{
    {"mail_items", "mail_id"}, //1
};

static const uint8 acc_tables_count = 1;
char *tab_acc[][2] =
{
    {"realmcharacters", "acctid"}, //1
};

static const uint8 acc_char_tables_count = 4;
char *tab_acc_char[][2] =
{
    {"characters", "account"}, //1
    {"account_data", "accountId"}, // 2
    {"account_tutorial", "accountId"}, // 3
    {"account_access", "id"}, // 4
};

/*
ALTER TABLE `character_inventory` ADD INDEX `bag` (`bag`);
ALTER TABLE `item_instance`
ADD INDEX `creatorGuid` (`creatorGuid`),
ADD INDEX `giftCreatorGuid` (`giftCreatorGuid`);
ALTER TABLE `mail`
ADD INDEX `sender` (`sender`);
ALTER TABLE `character_talent`
ADD INDEX `guid` (`guid`);
ALTER TABLE `character_inventory`
ADD INDEX `bag` (`bag`);

ALTER TABLE `guild`
ADD INDEX `leaderguid` (`leaderguid`);
*/