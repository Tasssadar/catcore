/*
GUID OVERFLOW FIXER
=============================================================
 - Changes biggest guids to lower which are free
 - look at line 36

 For compile use:
     g++ -o char_guid $(mysql_config --cflags) char_guid.cpp $(mysql_config --libs) -g -O2

*/

#include "char_guid.h"
char *buff_char = new char[500];

void printLog(const char *fmt,...)
{
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(buff_char,fmt,argptr);
  printf("%s", buff_char);
  fprintf(log_file, "%s", buff_char);
  fflush(stdout);
  va_end(argptr);
}

MYSQL *connectToDb(char* database)
{
    printLog("Connecting to database %s..\n", database);

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
    va_list argptr;
    va_start(argptr,query);
    vsprintf(buff_char,query,argptr);
    va_end(argptr);

    fprintf(log_file, "%s\n", buff_char);

    if (mysql_query(con, buff_char))
    {
        printLog("%s\n", mysql_error(con));
        return false;
    }
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

    bool succes = true;
    for(uint8 i = 0; i < REPEATING; ++i)
    {
        while(state < MAX_STEPS)
        {
            switch(state)
            {
                case 0: // characters
                {
                    printLog("        PROCESING CHARACTER GUIDs\n");
                    succes = changeGuids(tab_char, char_tables_count, OFFSET_CHARACTERS, conn_char);
                    time_t curTime = time(0);
                    printLog("Characters converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 1:
                {
                    printLog("        PROCESING GUILD GUIDs\n");
                    succes = changeGuids(tab_guild, guild_tables_count, OFFSET_GUILD, conn_char);
                    time_t curTime = time(0);
                    printLog("Guilds converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 2:
                {
                    printLog("        PROCESING ITEM GUIDs\n");
                    succes = changeGuids(tab_item, item_tables_count, OFFSET_ITEMS, conn_char);
                    time_t curTime = time(0);
                    printLog("Items converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 3:
                {
                    printLog("        PROCESING PET GUIDs\n");
                    succes = changeGuids(tab_pet, pet_tables_count, OFFSET_PETS, conn_char);
                    time_t curTime = time(0);
                    printLog("Pets converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                    break;
                }
                case 4:
                {
                    printLog("        PROCESING MAIL GUIDs\n");
                    succes = changeGuids(tab_mail, mail_tables_count, OFFSET_MAIL, conn_char);
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
                        succes = changeGuids(tab_acc, acc_tables_count, OFFSET_ACC, conn_acc, conn_char, tab_acc_char, acc_char_tables_count);
                        time_t curTime = time(0);
                        printLog("Accounts converted after %u seconds, total time is %u\n", curTime - time_c, curTime - startTime);
                        mysql_close(conn_acc);
                    }
                    else
                        printLog("Could not open connection to account database!");
                    break;
                }
            }
            if(!succes)
                break;
            
            time_c = time(0);
            incrementState();
        }
        printLog("Iteration %u of %u complete. Succes: %u\n", i+1, REPEATING, uint8(succes));
        if(!succes)
            break;
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

bool changeGuids(char *tables[][2], uint tables_count, uint offset, MYSQL *con,
                 MYSQL *con_acc, char *tables_acc[][2], uint tables_count_acc)
{
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;

    printLog("\nLOADING GUIDS FROM DB \n ");
    printLog("============================ \n");


    if (!my_query(con, "SELECT COUNT(%s) FROM %s;", tables[0][1], tables[0][0]))
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

    if (!my_query(con, "SELECT %s FROM %s ORDER BY %s;", tables[0][1], tables[0][0], tables[0][1]))
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
    printLog("max guid %u\n", target_guid);

    std::qsort(guids_orig, count, sizeof(uint), compare);
    std::qsort(guids_new, count, sizeof(uint), compare);

    for(uint i = 0; i < count; ++i)
    {
        if(guids_orig[i] == 0)
            continue;
        if(guids_orig[i] > offset)
            break;
        guids_new[i] = ++target_guid;
    }

    printLog("max guid %u\n", guids_orig[count-1]);
    //printLog("max guid %u\n", guids_new[count-1]);

    printLog("\nASSEMBLING FREE GUIDS \n ");
    printLog("============================ \n");
    
    std::list<uint> free;

    uint lastGuid = offset;
    uint y_free = 0;
    uint free_size = 0;
    for(uint i = 0; i < count; ++i)
    {
        if(guids_orig[i] == 0 || guids_orig[i] < offset)
            continue;

        if(free_size > count)
            break;
        if(guids_orig[i] - lastGuid > 1)
        {
           // printLog("%u %u\n", guids_new[i], lastGuid);
            ++lastGuid;
            for(;lastGuid < guids_orig[i] && free_size <= count; ++lastGuid)
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
                {
                 //   printLog("%u\n", lastGuid);
                    free.push_back(lastGuid);
                    ++free_size;
                }
            }
        }
        else
            ++lastGuid;

        printf("\r%u %u%% complete", free.size(), uint(free.size()/(float(count)/100.0f)));
        fflush(stdout);
    }

    printLog("\n\nPREPARING GUIDS\n ");
    printLog("============================ \n");
    printLog("begin: %u, %u \n", guids_new[0], *(free.begin()));

    

    for(long i = count-1; i > 0; --i)
    {
        if(guids_new[i] == 0 || guids_new[i] <= *(free.begin()))
            continue;

        if(free.empty())
        {
            printLog("Break - %u %u\n", guids_new[i], *(free.begin()));
            break;
        }

       // printLog("%u %u -> %u\n", guids_orig[i], guids_new[i], *(free.begin()));
        guids_new[i] = *(free.begin());

        free.pop_front();
        if(free.empty())
        {
     //       printLog("Break - empty()");
            break;
        }
    }

    printLog("\nSTATS\n ");
    printLog("============================ \n");
    uint updates_count = 0;
    for(uint i = 0; i < count; ++i)
    {
        if(guids_new[i] == 0 || guids_orig[i] == guids_new[i])
            continue;
        ++updates_count;
    }
    printLog("Total guids: %u total changes: %u (%u%%)\n", count, updates_count, uint(updates_count/(float(count)/100.0f)));
    float guids_pct = float(updates_count)/100.0f;

    printLog("\nCREATING PREPARED STATEMENTS\n ");
    printLog("============================ \n");
    
    {
        for(uint8 y = 0; y < tables_count; ++y)
             my_query(con, "PREPARE %s_%s_stm FROM \"UPDATE %s SET %s=? WHERE %s=?;\";",  tables[y][0], tables[y][1],  tables[y][0], tables[y][1], tables[y][1]);
        if(con_acc)
        {
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "PREPARE %s_%s_stm FROM \"UPDATE %s SET %s=? WHERE %s=?;\";",  tables_acc[y][0],  tables_acc[y][1], tables_acc[y][0], tables_acc[y][1], tables_acc[y][1]);
        }
    }
    
    printLog("\nUPDATING DATABASE\n ");
    printLog("============================ \n");

    uint counta = 0;
    uint guid_with_off = 0;
    time_t curT = time(0);
    time_t lastT = time(0);
    uint lastTcount = 0;
    float per_sec = 0;
    uint eta = 0;

    for(long i = count-1; i > 0; --i)
    {
        if(guids_new[i] == guids_orig[i] || guids_new[i] == 0 || guids_orig[i] == 0)
            continue;

        guid_with_off = guids_new[i];
        //printLog("%u %u\n", itr->first, guid_with_off);

        my_query(con, "START TRANSACTION;");
        my_query(con, "SET @guid_from = '%u';", guids_orig[i]);
        my_query(con, "SET @guid_to = '%u';", guid_with_off);

        for(uint8 y = 0; y < tables_count; ++y)
            my_query(con, "EXECUTE %s_%s_stm USING @guid_to, @guid_from;", tables[y][0], tables[y][1]);

        if(con_acc)
        {
            my_query(con_acc, "START TRANSACTION;");
            my_query(con_acc, "SET @guid_from = '%u';", guids_orig[i]);
            my_query(con_acc, "SET @guid_to = '%u';", guid_with_off);
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "EXECUTE %s_%s_stm USING @guid_to, @guid_from;", tables_acc[y][0], tables_acc[y][1]);
            my_query(con_acc, "COMMIT;");
        }

        my_query(con, "COMMIT;");

        curT = time(0);
        if(curT - lastT >= 5)
        {
            per_sec = float(counta-lastTcount)/5.0f;
            lastT = curT;
            lastTcount = counta;
            eta = (updates_count-counta)/per_sec;
        }
        fprintf(log_file, "\n    presunul %u -> %u\n", guids_orig[i], guid_with_off);
        printf("\r %u %u%% complete, %3.2f guid per second, ETA: %us", counta, uint(counta/guids_pct), per_sec, eta);
        fflush(stdout);
 
        ++counta;
    }

    delete[] guids_new;
    delete[] guids_orig;

    printLog("\nREMOVING PREPARED STATEMENTS\n ");
    printLog("============================ \n");

    {
        for(uint8 y = 0; y < tables_count; ++y)
             my_query(con, "DEALLOCATE PREPARE %s_%s_stm;", tables[y][0], tables[y][1]);
        if(con_acc)
        {
            for(uint8 y = 0; y < tables_count_acc; ++y)
                my_query(con_acc, "DEALLOCATE PREPARE %s_%s_stm;", tables_acc[y][0], tables_acc[y][1]);
        }
    }
    printf("\n");

    return true;
}

