/* ======================================================================================================== */
/* SQL.H - Logon and Write records to MYSQL table defines													*/
/*																											*/
/* Author: Andy Wysocki   03/16/2013																		*/
/*																											*/
/* This code is release under the Open Software License v. 3.0 (OSL-3.0)									*/
/*					http://opensource.org/licenses/OSL-3.0													*/
/*																											*/
/*	Release 1.0 - 03/15/2012																				*/
/*																											*/
/* ======================================================================================================== */
#ifndef SQLQOD_H
#define SQLQOD_H


enum VAL {
  SQL_ERR_NOERROR=0,
  SQL_ERR_MYSQLINIT=-1,
  SQL_ERR_CONNECTFAILED=-2,
  SQL_ERR_FAILEDINSERT=-3,
  };



int     InitLogonSQL( char *_User, char * _Pass, char *_DB, char *_Host );
int		WriteXML2DB( char *_table, char *_XML );




#endif
