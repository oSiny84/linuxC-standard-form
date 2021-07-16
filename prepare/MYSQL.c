#include <mysql.h>
#include <my_global.h>
 
#include <string.h>
 
#include "MYSQL.h"

char DB_HOST[] = "buaniot.cafe24.com";
char DB_USER[] = "buaniot";
char DB_PASS[] = "ftppiluhak123";
char DB_NAME[] = "buaniot";

void insert(char str[]){
   
    MYSQL *con = mysql_init(NULL);
    char query[600];

    if (con == NULL)
    {
        printf("[connect error] %s\n", mysql_error(con));
        return;
    }

    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0) == NULL)
    {
        printf("[Rconnect error] %s\n", mysql_error(con));
        mysql_close(con);
        return;
    }
    sprintf( query, SQL_INSERT_RECORD, str );
    if (mysql_query(con, query))
    {
        printf("[query error : %s] %s\n", query, mysql_error(con));
        mysql_close(con);
        return;
    }else{
        printf("[query] %s\n", str);
    }
 
    mysql_close(con);
}
