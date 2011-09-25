/*
GUID OVERFLOW FIXER
=============================================================
 - Changes biggest guids to lower which are free
 - look at line 36

 For compile use:
     g++ -o char_glyphs $(mysql_config --cflags) char_glyphs.cpp $(mysql_config --libs) -g

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
#include <map>

using namespace std;

struct tables
{    std::string name;
    std::string column;
};

struct glyphs_t
{
    uint glyphs[2][6];
    void reset()
    {
        for(char i = 0; i< 6; ++i)
        {
            glyphs[0][i] = 0;
            glyphs[1][i] = 0;
        }
    }
};

int main()
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    std::stringstream file;
    file << "char_glyphs_log_" << time(NULL);
    FILE *log = fopen(file.str().c_str(), "a");

    char *server = "valhalla-team.com";
    char *user = "tass";
    char *password = "0pt1musOn3"; /* set me first */
    char *database = "mg2";

    conn = mysql_init(NULL);

    /* Connect to database */
    if (!mysql_real_connect(conn, server,
     user, password, database, 0, NULL, 0)) {
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    if (mysql_query(conn, "SELECT * FROM character_glyphs_old")) {
        fprintf(log, "%s\n", mysql_error(conn));
        return 1;
    }

    res = mysql_use_result(conn);
    std::map<uint, glyphs_t*> glyph_map;
    std::map<uint, glyphs_t*>::iterator glyph_itr;

    printf("\nLOADING GLYPHS FROM DB \n ");
    printf("============================ \n");
    fprintf(log, "\nLOADING GLYPHS FROM DB \n ");
    fprintf(log, "============================ \n");

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        uint guid = atoi(row[0]);
        glyph_itr = glyph_map.find(guid);
        if(glyph_itr == glyph_map.end())
        {
            glyphs_t *glyphs = new glyphs_t;
            glyphs->reset();
            glyphs->glyphs[atoi(row[1])][atoi(row[2])] = atoi(row[3]);
            glyph_map.insert(std::make_pair<uint, glyphs_t*>(guid, glyphs));
        }
        else
            glyph_itr->second->glyphs[atoi(row[1])][atoi(row[2])] = atoi(row[3]);
    }
    mysql_free_result(res);
    uint counter = 0;
    for(glyph_itr = glyph_map.begin(); glyph_itr != glyph_map.end();)
    {
        char * str = new char[2000];
        std::stringstream ss;
        sprintf(str, "INSERT INTO `character_glyphs` (`guid`, `spec`, `glyph1`, `glyph2`, `glyph3`, `glyph4`, `glyph5`, `glyph6`) VALUES \n");
        ss << str;
        
        for(char y = 0; y < 25 && glyph_itr != glyph_map.end(); ++y)
        {
            uint *loc[2] = { glyph_itr->second->glyphs[0], glyph_itr->second->glyphs[1]};
            for(char i = 0; i < 2; ++i)
            {
                sprintf(str, "('%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u')", glyph_itr->first, i, loc[i][0], loc[i][1], loc[i][2], loc[i][3], loc[i][4], loc[i][5]);
                ss << str;
                if(i == 0)
                    ss << ",\n";
            }
            ++glyph_itr;
            if(y == 24 || glyph_itr == glyph_map.end())
                ss << ";";
            else
                ss << ",\n";
            ++counter;
            uint percent = float(counter)/(float(glyph_map.size())/100.f);
            printf("\r%u %% complete", percent);
            fflush(stdout);
        }
        fprintf(log, "%s\n\n", ss.str().c_str());
        mysql_query(conn, ss.str().c_str());
        delete[] str;
    }

    printf("\n\n");

    mysql_close(conn);
    fclose(log);
}

