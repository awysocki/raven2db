/* ======================================================================================================== */
/* PARSE.H - Read INI file																					*/
/*																											*/
/* Author: Andy Wysocki   03/16/2013																		*/
/*																											*/
/* This code is release under the Open Software License v. 3.0 (OSL-3.0)									*/
/*					http://opensource.org/licenses/OSL-3.0													*/
/*																											*/
/*	Release 1.0 - 03/15/2012																				*/
/*																											*/
/* ======================================================================================================== */
/* ==========================================================================*/
/* PARSE.H - Defines for PARSE INI Code                                      */
/* ==========================================================================*/

int
GetIniString(char *_AppName,
             char *_KeyName,
             char *_Default,
             char *_String,
             int   _StringSize,
             char *_FileName);

int
WriteIniString(char *_AppName,
             char *_KeyName,
             char *_String,
             char *_FileName);

int   CheckYes( char *_Check );
int   CheckNo( char *_Check );

#define INI_SUCCESSFUL          0               /* App,Key found        */
#define INI_FNF                 1               /* File Not Found       */
#define INI_ANF                 2               /* AppName not Found    */
#define INI_KNF                 3               /* Key Not found        */
