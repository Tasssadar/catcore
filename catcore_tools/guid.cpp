/*
GUID OVERFLOW FIXER
=============================================================
 *Changes biggest guids to lower which are free

*/

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

using namespace std;

struct tables
{
    std::string name;
    std::string column;
};

int main()
{
    // Leave or change guid in other tables, if false then skip creatures which have record in other tables
    bool changeGUID = false;

    static tables tabulky[]=
    {
        {"creature_addon", "guid"},
        {"creature_battleground", "guid"},
        {"creature_movement", "id"},
        {"game_event_creature", "guid"},
        {"game_event_model_equip", "guid"},
        {"npc_gossip", "npc_guid"},
        {"pool_creature", "guid"}
    };
                            
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    std::stringstream file;
    file << "guidlog_";
    file << time(NULL);
    FILE *log = fopen(file.str().c_str(), "a");

    char *server = "localhost";
    char *user = "user";
    char *password = "pass"; /* set me first */
    char *database = "database";
    
    conn = mysql_init(NULL);
               
    /* Connect to database */
    if (!mysql_real_connect(conn, server,
     user, password, database, 0, NULL, 0)) {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    if (mysql_query(conn, "SELECT guid FROM creature ORDER BY guid")) {
        fprintf(log, "%s\n", mysql_error(conn));
        return 1;
    }
    
    res = mysql_use_result(conn);
    std::list<long> guids;

    printf("\nLOADING GUIDS FROM DB \n ");
    printf("============================ \n");
    fprintf(log, "\nLOADING GUIDS FROM DB \n ");
    fprintf(log, "============================ \n");
        
    
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        int a = atoi(row[0]);
        guids.push_back(long(a));
    }
    mysql_free_result(res);

    printf("\nASSEMBLING FREE GUIDS \n ");
    printf("================================ \n");
    fprintf(log, "\nASSEMBLING FREE GUIDS \n ");
    fprintf(log, "================================= \n");        
    std::list<long> free;

    long lastGuid = *(guids.begin())-1;
    fprintf(log, "firstguid %u \n", *(guids.begin()));
    for(std::list<long>::iterator itr = guids.begin(); itr != guids.end(); ++itr)
    {
        if((*itr) - lastGuid > 1)
        {
            ++lastGuid;
            for(;lastGuid < (*itr); ++lastGuid)
               free.push_back(lastGuid);
        }
        else
            ++lastGuid;
    }
    
    printf("\nCLEANING UP 0 GUID \n ");
    printf("============================ \n");
    fprintf(log, "\nCLENING UP 0 GUID \n ");
    fprintf(log, "============================ \n");
    //Cleanup...
    mysql_query(conn, "SELECT guid FROM creature WHERE guid=0");
    
    res = mysql_use_result(conn);
    row = mysql_fetch_row(res);
    if(row != NULL)
    {
        printf("\n coze? \n");
        std::stringstream ss;
        ss << "UPDATE creature SET guid = ";
        ss << *(free.begin());
        ss <<  " WHERE guid=0"; 
        fprintf(log, "\n %s \n", ss.str().c_str());
        mysql_free_result(res);
        mysql_query(conn, ss.str().c_str());
        free.pop_front();
        guids.pop_front();
    } 
    fprintf(log, " \n error %s \n", mysql_error(conn));
        guids.reverse();
     bool can = true;
    int counta = 1;
    
    printf("\nCHANGING GUIDS \n ");
    printf("============================ \n");
    fprintf(log, "\nCHANGING GUIDS \n ");
    fprintf(log, "============================ \n");    
    fprintf(log, "\n begin: %u, %u \n", *(guids.begin()), *(free.begin()));
    
    for(std::list<long>::iterator itr = guids.begin(); itr != guids.end(); ++itr)
    {
        can = true;
        if(*itr <= *(free.begin()))
            break;
        for(int y = 0; y < 6; ++y)
        {
             std::stringstream ss;
             ss << "SELECT ";
             ss << tabulky[y].column;
             ss << " FROM ";
             ss << tabulky[y].name;
             ss << " WHERE ";
             ss << tabulky[y].column;
             ss <<  "=";
             ss << *itr;
             mysql_query(conn, ss.str().c_str());
             res = mysql_use_result(conn);
             if(!res)
             {
              mysql_free_result(res);
              continue;
             }
             if(row = mysql_fetch_row(res))
             {
                 if(changeGUID)
                 {
                    fprintf(log, "\n edit %s for creature %u", (tabulky[y].name).c_str(), *itr);
                    mysql_free_result(res);
                    std::stringstream sss;
                    sss << "UPDATE ";
                    sss << tabulky[y].name;
                    sss << " SET ";
                    sss << tabulky[y].column;
                    sss << "=";
                    sss << *(free.begin());
                    sss << " WHERE ";
                    sss << tabulky[y].column;
                    sss << "=";
                    sss << *itr;
                    mysql_query(conn, sss.str().c_str());
                 }
                 else
                 {
                    can = false;
                    mysql_free_result(res);
                    break;
                 }
             }else
                 mysql_free_result(res);
        }
        if(!can)
            continue;
        std::stringstream ss;
        ss << "UPDATE creature SET guid = ";
        ss << *(free.begin());
        ss << " WHERE guid =  ";
        ss << *itr;
        fprintf(log, "\n \n %s", ss.str().c_str());
        
        
        mysql_query(conn, ss.str().c_str()); 

        fprintf(log, "\n    presunul %u -> %u", *itr, *(free.begin()));
        free.pop_front();
        if(free.empty())
            break;
        ++counta;
    } 
    long maxguid = 0;
 
    mysql_query(conn, "SELECT MAX(guid) FROM creature");
    
    res = mysql_use_result(conn);
    row = mysql_fetch_row(res);
    if(row != NULL)
        maxguid = atoi(row[0]);

    printf("\n Changed creatures: %u, maxguid: %u, space between overflow: %u\n", counta, maxguid, (0x00FFFFFF - maxguid));
    mysql_free_result(res);
    mysql_close(conn);
    fclose(log);
}

