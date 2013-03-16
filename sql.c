/* ======================================================================================================== */
/* SQL.C - Logon and Write records to MYSQL table															*/
/*																											*/
/* Author: Andy Wysocki   03/16/2013																		*/
/*																											*/
/* This code is release under the Open Software License v. 3.0 (OSL-3.0)									*/
/*					http://opensource.org/licenses/OSL-3.0													*/
/*																											*/
/*	Release 1.0 - 03/15/2012																				*/
/*																											*/
/* ======================================================================================================== */

#include <my_global.h>
#include <my_sys.h>
#include <m_string.h>
#include "mysql.h"
#include "mysql_version.h"
#include "mysqld_error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "sql.h"
#include "dbglog.h"


MYSQL           gMySQL;
MYSQL           gMySQLWrite;
char            gSQLBuff[1024 * 64];


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int
InitLogonSQL( char * _SQLUser, char * _SQLPassword, char * _SQLDatabase, char *_SQLHost )
{
char    locbuf[255];
MYSQL	*rc;

	rc = mysql_init( &gMySQL );
	if ( !rc )
	{
		WriteDBGLog( "mysql_init failed" );
		return( SQL_ERR_MYSQLINIT );
	}

	if (!mysql_real_connect( &gMySQL, _SQLHost,
		_SQLUser, _SQLPassword, _SQLDatabase,
		0, NULL, 0 ))
	{
		sprintf( locbuf, "Failed to connect to database: Error %s\n", mysql_error( &gMySQL) );
		WriteDBGLog( locbuf );
		return( SQL_ERR_CONNECTFAILED );
	}

	return( SQL_ERR_NOERROR );

}




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int
WriteXML2DB( char *_table, char *_XML )
{
    //int     rc=0;
    //int	i;
    
    
    sprintf( gSQLBuff, "INSERT INTO %s SET `xml`='%s'", _table, _XML  );
    
    
    if ( !mysql_query( &gMySQL, gSQLBuff ) )
    {
        return( SQL_ERR_NOERROR );
    }
    else
    {
		WriteDBGLog( "Insert Failed" );
        WriteDBGLog( gSQLBuff );
    }
    
    return(SQL_ERR_FAILEDINSERT);
}
