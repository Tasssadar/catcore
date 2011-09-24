/*
GUID OVERFLOW FIXER
=============================================================
 - Changes biggest guids to lower which are free
 - look at line 36

 For compile use:
     g++ -o char_guid $(mysql_config --cflags) char_guid.cpp $(mysql_config --libs) -g -O2

*/

#include "char_guid.h"

void printLog(const char *fmt,...)
{
  char *msg = new char[500];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(msg,fmt,argptr);
  printf("%s", msg);
  fprintf(log_file, "%s", msg);
  fflush(stdout);
  va_end(argptr);
  delete[] msg;
}

MYSQL *connectToDb(char* database)
{
    printLog("Connecting to database %s..\n", database);
    //char *socket_loc = NULL;
    //if(!server)
      //  socket_loc = socket;

    MYSQL *con = mysql_init(NULL);
    if (!mysql_real_connect(con, server, user, password, database, 0, NULL, 0))
    {
        printLog("%s\n", mysql_error(con));
        return NULL;
    }
    return con;
}

bool my_query(MYSQL *con, char *query,...)
{
    char *buff = new char[500];

    va_list argptr;
    va_start(argptr,query);
    vsprintf(buff,query,argptr);
    va_end(argptr);

    fprintf(log_file, "%s\n", buff);

    if (mysql_query(con, buff))
    {
        printLog("%s\n", mysql_error(con));
        delete[] buff;
        return false;
    }
    delete[] buff;
    return true;
}

void incrementState()
{
    ++state;
    rewind(state_file);
    fwrite(&state, 1, 1, state_file);
    fflush(state_file);
}


int main()
{
    std::stringstream file;
    file << "char_log_" << time(NULL);
    log_file = fopen(file.str().c_str(), "a");

    printLog("Char guid converter started, loading saved state file...\n");
    state_file = fopen("char_guid_state", "r+");
    if(!state_file)
    {
        printLog("State file does not exist, creating one..\n");
        state_file = fopen("char_guid_state", "w+");
        rewind(state_file);
        fwrite(&state, 1, 1, state_file);
        fflush(state_file);
    }
    else
    {
        fread(&state, 1, 1, state_file);
        printLog("State file found, state %u loaded\n", state);
    }

    conn_char = connectToDb(database_char);
    if(!conn_char)
    {
        fclose(log_file);
        fclose(state_file);
        return 0;
    }
    time_t startTime = time(0);
    time_t time_c = time(0);
    printLog("Connected\n");
    for(uint8 i = 0; i < REPEATING; ++i)
    {
        while(state < MAX_STEPS)
        {
            switch(state)
            {
                case 0: // characters
                {
                    printLog("        PROCESING CHARACTER GUIDs\n");
                    changeGuids("characters", "guid", tab_char, char_tables_count, OFFSET_CHARACTERS, conn_char);
                    time_t curTime = time(0);
                    printLog("Characters converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 1:
                {
                    printLog("        PROCESING GUILD GUIDs\n");
                    changeGuids("guild", "guildid", tab_guild, guild_tables_count, OFFSET_GUILD, conn_char);
                    time_t curTime = time(0);
                    printLog("Guilds converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 2:
                {
                    printLog("        PROCESING ITEM GUIDs\n");
                    changeGuids("item_instance", "guid", tab_item, item_tables_count, OFFSET_ITEMS, conn_char);
                    time_t curTime = time(0);
                    printLog("Items converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 3:
                {
                    printLog("        PROCESING PET GUIDs\n");
                    changeGuids("character_pet", "id", tab_pet, pet_tables_count, OFFSET_PETS, conn_char);
                    time_t curTime = time(0);
                    printLog("Pets converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 4:
                {
                    printLog("        PROCESING MAIL GUIDs\n");
                    changeGuids("mail", "id", tab_mail, mail_tables_count, OFFSET_MAIL, conn_char);
                    time_t curTime = time(0);
                    printLog("Mails converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 5:
                {
                    conn_acc = connectToDb(database_acc);
                    if(conn_acc)
                    {
                        printLog("        PROCESING ACCOUNT GUIDs\n");
                        changeGuids("account", "id", tab_acc, acc_tables_count, OFFSET_ACC, conn_acc, conn_char, tab_acc_char, acc_char_tables_count);
                        time_t curTime = time(0);
                        printLog("Accounts converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                        mysql_close(conn_acc);
                    }
                    else
                        printLog("Could not open connection to account database!");
                    break;
                }
            }
            time_c = time(0);
            incrementState();
        }
        printLog("Iteration %u of %u complete\n", i+1, REPEATING);
        state = 0;
        rewind(state_file);
        fwrite(&state, 1, 1, state_file);
        fflush(state_file);
    }
    fclose(log_file);
    fclose(state_file);
    mysql_close(conn_char);
    return 0;
}

bool changeGuids(char *main_table, char *main_col, char *tables[][2], uint tables_count, uint offset, MYSQL *con,
                 MYSQL *con_acc, char *tables_acc[][2], uint tables_count_acc)
{
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;

    printLog("\nLOADING GUIDS FROM DB \n ");
    printLog("============================ \n");


    if (!my_query(con, "SELECT COUNT(%s) FROM %s;", main_col, main_table))
        return false;

    res = mysql_use_result(con);
    uint count = atoi(mysql_fetch_row(res)[0]) + 1;
    printLog("%u guids\n", count);
    mysql_free_result(res);
    
    uint *guids_orig = new uint[count];
    uint *guids_new = new uint[count];
    for(uint i = 0; i < count; ++i)
    {
        guids_new[i] = 0;
        guids_orig[i] = 0;
    }

    if (!my_query(con, "SELECT %s FROM %s ORDER BY %s;", main_col, main_table, main_col))
        return false;

    res = mysql_use_result(con);

    uint a;
    uint real_guid_count = 0;
    for(uint i = 0; (row = mysql_fetch_row(res)) != NULL; ++i)
    {
        a = atoi(row[0]);
        guids_new[i] = a;
        guids_orig[i] = a;
        ++real_guid_count;
    }
    mysql_free_result(res);

    uint target_guid = (guids_new[count-2] > offset) ? guids_new[count-2] : offset;

    for(uint i = 0; i < count; ++i)
    {
        if(guids_orig[i] == 0)
            continue;
        if(guids_orig[i] > offset)
            break;
        guids_new[i] = ++target_guid;
    }
    std::sort(guids_new, guids_new+sizeof(guids_new) / sizeof(guids_new[0]));

    printLog("\nASSEMBLING FREE GUIDS \n ");
    printLog("============================ \n");
    
    std::list<uint> free;

    uint lastGuid = offset;
    uint y_free = 0;
    for(uint i = 0; i < count; ++i)
    {
        if(guids_new[i] == 0)
            continue;
        if(free.size() > count)
            break;
        if(guids_new[i] - lastGuid > 1)
        {
            ++lastGuid;
            for(;lastGuid < guids_new[i]; ++lastGuid)
            {
                bool can = true;
                for(; y_free < count; ++y_free)
                {
                    if(guids_orig[y_free] == lastGuid || guids_new[y_free] == lastGuid)
                    {
                        can = false;
                        break;
                    }
                    if(guids_orig[y_free] > lastGuid && guids_new[y_free] > lastGuid)
                        break;
                }
                if(can)
                    free.push_back(lastGuid);
            }
            
        }
        else
            ++lastGuid;

        printf("\r%u %u%% complete", free.size(), uint(free.size()/(float(count)/100.0f)));
        fflush(stdout);
    }

    printLog("\nPREPARING GUIDS\n ");
    printLog("============================ \n");
    printLog("begin: %u, %u \n", guids_new[0], *(free.begin()), real_guid_count);

    float guids_pct = float(count)/100.0f;

    for(long i = count-1; i > 0; --i)
    {
        if(guids_new[i] == 0)
            continue;

        if(free.empty() || guids_new[i] <= *(free.begin()))
            break;
        guids_new[i] = *(free.begin());

        free.pop_front();
        if(free.empty())
            break;
    }
    std::sort(guids_new, guids_new+sizeof(guids_new) / sizeof(guids_new[0]));
    std::sort(guids_orig, guids_orig+sizeof(guids_orig) / sizeof(guids_orig[0]));

    printLog("\nCREATING PREPARED STATEMENTS\n ");
    printLog("============================ \n");
    
    {
        my_query(con, "PREPARE %s_stm FROM \"UPDATE %s SET %s=? WHERE %s=?;\";", main_table, main_table, main_col, main_col);
        for(uint8 y = 0; y < tables_count; ++y)
             my_query(con, "PREPARE %s_stm FROM \"UPDATE %s SET %s=? WHERE %s=?;\";",  tables[y][0],  tables[y][0], tables[y][1], tables[y][1]);
        if(con_acc)
        {
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "PREPARE %s_stm FROM \"UPDATE %s SET %s=? WHERE %s=?;\";",  tables_acc[y][0],  tables_acc[y][0], tables_acc[y][1], tables_acc[y][1]);
        }
    }
    
    printLog("\nUPDATING DATABASE\n ");
    printLog("============================ \n");

    uint counta = 0;
    uint guid_with_off = 0;

    for(long i = count-1; i > 0; --i)
    {
        if(guids_new[i] == 0 || guids_orig[i] == 0)
            continue;
        if(guids_new[i] == guids_orig[i])
        {
            printf("\r %u %u%% complete", counta, uint(counta/guids_pct));
            ++counta;
            continue;
        }
        guid_with_off = guids_new[i];
        //printLog("%u %u\n", itr->first, guid_with_off);

        my_query(con, "START TRANSACTION;");
        my_query(con, "SET @guid_from = '%u';", guids_orig[i]);
        my_query(con, "SET @guid_to = '%u';", guid_with_off);

        my_query(con, "EXECUTE %s_stm USING @guid_to, @guid_from;", main_table);
        for(uint8 y = 0; y < tables_count; ++y)
            my_query(con, "EXECUTE %s_stm USING @guid_to, @guid_from;", tables[y][0]);

        if(con_acc)
        {
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "EXECUTE %s_stm USING @guid_to, @guid_from;", tables_acc[y][0]);
        }

        my_query(con, "COMMIT;");

        fprintf(log_file, "\n    presunul %u -> %u\n", guids_orig[i], guid_with_off);
        printf("\r %u %u%% complete", counta, uint(counta/guids_pct));
        fflush(stdout);

        ++counta;
    }
    delete[] guids_new;
    delete[] guids_orig;

    printLog("\nREMOVING PREPARED STATEMENTS\n ");
    printLog("============================ \n");

    {
        my_query(con, "DEALLOCATE PREPARE %s_stm;", main_table);
        for(uint8 y = 0; y < tables_count; ++y)
             my_query(con, "DEALLOCATE PREPARE %s_stm;", tables[y][0]);
        if(con_acc)
        {
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "DEALLOCATE PREPARE %s_stm;", tables_acc[y][0]);
        }
    }
    printf("\n");

    return true;
}

