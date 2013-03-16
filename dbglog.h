/* ======================================================================================================== */
/* DBGLOG.H - Write to a Debug LOG file																		*/
/*																											*/
/* Author: Andy Wysocki   03/16/2013																		*/
/*																											*/
/* This code is release under the Open Software License v. 3.0 (OSL-3.0)									*/
/*					http://opensource.org/licenses/OSL-3.0													*/
/*																											*/
/*	Release 1.0 - 03/15/2012																				*/
/*																											*/
/* ======================================================================================================== */

#ifndef DBGLOG_H
#define DBGLOG_H

void InitDBGLog( char *_Key, char * _FileName, int _Debug, int _Verbose );
void WriteDBGLog( char *_Str );

#endif
