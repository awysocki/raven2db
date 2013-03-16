/* ======================================================================================================== */
/* RAVEN2DB.C - Read a USB Com port and write XML read into MySQL database table							*/
/*																											*/
/* Author: Andy Wysocki   03/16/2013																		*/
/*																											*/
/* This code is release under the Open Software License v. 3.0 (OSL-3.0)									*/
/*					http://opensource.org/licenses/OSL-3.0													*/
/*																											*/
/*	Release 1.0 - 03/15/2012																				*/
/*																											*/
/* ======================================================================================================== */

#include <stdarg.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse.h"
#include "sql.h"
#include "dbglog.h"

#include <stdio.h>
#include <errno.h>
//#include <termios.h> // POSIX terminal control definitionss
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <dirent.h>
#include <string.h>


enum {
	SUCCESSFUL = 0,
	ERR_BADFDOPEN = -1,
	ERR_BADFHOPEN= -2,
	ERR_BADPARM = -3,
	ERR_DBLOGON = -4,
};

char	*gStartUp = "\n\nraven2db - AB Software LLC\nVersion 0.1 Mar 12, 2013\nRead COM port for Rainforest RAVEn device and store data in MySQL DB\r\n\r\n";

char    *gApp = "Raven2DB";
char    *gINIFile="raven2db.ini";


char    gSQLUser[32];
char    gSQLPassword[32];
char    gSQLDataBase[32];
char    gSQLTable[32];
char	gSQLHost[128];

char	gDBGVerbose = 0;
char    gDebugLog[255];
char    gDebug;
char	gDbgBuf[2048];


char	gReadBuf[1024];
char	gXMLBuffer[10*1024];
int		gXMLMaxBufferLen = sizeof( gXMLBuffer );
int		gXMLBufferLen;

char	gUSBDev[128];

int		gFD;
FILE	*gFH;


char	gCmdBuffer[1024*5];
int		gCmdBufferLen;

#define READ_BINARY	"rb"
#define WRITE_BINARY "wb"


//----------------------------------------
//----------------------------------------
void LoadINIParms( );
void ProcessData ( void );
int	OpenPort( void );
int ConfigurePort( int );
int	FormatCmdBuffer( char *_cmd );


//----------------------------------------
//----------------------------------------
int
main (argc, argv, envp)
int argc;
char *argv[];
char *envp;
{
	int		c;
	int		rc;
	char	buf[1024];


	while( ( c = getopt( argc, argv, "v?h" ) ) != -1 )
	{
		switch( c )
		{

		case 'v':
			gDBGVerbose = 1;
			break;

		case '?':
		case 'h':
			printf( gStartUp );
			printf( "\r\n-v - VERBOSE print everything that would go to DEBUG LOG if debug was turned on\r\n" );
			exit(SUCCESSFUL);
			break;

		default:
			printf("? Unrecognizable switch [%s] - program aborted\n", optarg );
			exit( ERR_BADPARM );
		}
	}

	LoadINIParms();               // Get my custom parms

	InitDBGLog( "RAVEn", gDebugLog, gDebug, gDBGVerbose);

	WriteDBGLog( gStartUp );


	sprintf(gDbgBuf, "Logging on with user %s, pw=%s, DB=%s Host=%s\n", gSQLUser, gSQLPassword, gSQLDataBase, gSQLHost );
	WriteDBGLog( gDbgBuf );

	if ( rc = InitLogonSQL( gSQLUser, gSQLPassword, gSQLDataBase, gSQLHost ) )
	{
		sprintf(gDbgBuf, "Error Logging on with user %s, pw=%s, DB=%s Host=%s\nrc=%d", gSQLUser, gSQLPassword, gSQLDataBase, gSQLHost, rc );
		WriteDBGLog( gDbgBuf );
		exit( ERR_DBLOGON );
	}



	rc = OpenPort();
	
	if ( !rc )
	{

		SendCmd( "initialize" );
		WriteDBGLog( "Wrote INITALIZE xml to port" );


		ProcessData();
	}


	return rc;

}

/*===========================================================================*/
/* SENDCMD - Send a RAVEn command to the device								 */
/*===========================================================================*/
int
SendCmd( char *_cmd )
{
	int		len;

	if ( strlen( _cmd ) > 1024 )
		return ( -1);

	memset( gCmdBuffer, 0, sizeof( gCmdBuffer) );
	gCmdBufferLen = sprintf( gCmdBuffer, "<Command>\r\n  <Name>%s</Name>\r\n</Command>\r\n", _cmd );

	WriteDBGLog( gCmdBuffer );

	len = write( gFD, gCmdBuffer, gCmdBufferLen );

	if ( len != gCmdBufferLen )
	{
		sprintf( gDbgBuf, "Error writing Command to RAVEn port, %d != %d", len, gCmdBufferLen );
		WriteDBGLog( gDbgBuf );
		return( -1 );
	}

	return( 0 );
}

/*===========================================================================*/
/* LOADINIPARMS - Load in the INI file                                       */
/*===========================================================================*/
void
LoadINIParms( )
{
	char locbuf[256];

	int		rc = 0;
	/* Get the GLOBAL Section */
	GetIniString( gApp, "SQLUser", "", gSQLUser, sizeof( gSQLUser ), gINIFile );
	GetIniString( gApp, "SQLPassword", "", gSQLPassword, sizeof( gSQLPassword ), gINIFile );
	GetIniString( gApp, "SQLDataBase", "raven", gSQLDataBase, sizeof( gSQLDataBase ), gINIFile );
	GetIniString( gApp, "SQLTable", "ravenxml", gSQLTable, sizeof( gSQLTable ), gINIFile );
	GetIniString( gApp, "SQLHost", "localhost", gSQLHost, sizeof( gSQLHost ), gINIFile );

	GetIniString( gApp, "DebugLog", "./raven2db.log", gDebugLog, sizeof( gDebugLog ), gINIFile );
	GetIniString( gApp,   "Debug", "No", locbuf, sizeof( locbuf ), gINIFile );
	gDebug = CheckYes( locbuf );

	GetIniString( gApp, "USBDev", "/dev/ttyUSB0", gUSBDev, sizeof( gUSBDev ), gINIFile );


}

/*===========================================================================*/
/* OPENPORT - Open the USB Port for reading and writing						 */
/*===========================================================================*/
int OpenPort(void)
{
	int fd; // file description for the serial port
	
	sprintf( gDbgBuf, "Opening port [%s]", gUSBDev );
	WriteDBGLog( gDbgBuf );

	gFD = open(gUSBDev, O_RDWR);
	
	if(gFD == -1) // if open is unsucessful
	{
		sprintf(gDbgBuf, "open_port: Unable to open %s. 0x%0x - %s\n", gUSBDev, errno, strerror( errno ) );
		WriteDBGLog( gDbgBuf );
		perror( gDbgBuf );
		return( ERR_BADFDOPEN );
	}
	else
	{
		gFH = fdopen( gFD, "r" );

		if ( gFH == NULL )
		{
			sprintf(gDbgBuf, "open_port: Unable to open %s. 0x%0x - %s\n", gUSBDev, errno, strerror( errno ) );
			WriteDBGLog( gDbgBuf );
			perror( gDbgBuf );
			return( ERR_BADFHOPEN );

		}


		WriteDBGLog( "Port is open, both File Descriptor and File Handle" );

	}
	
	return(SUCCESSFUL);
} //open_port



/*===========================================================================*/
/* PROCESSDATA - Process the usb com port and store XML data into SQL DB     */
/*===========================================================================*/
void
ProcessData(  )
{
	int rblen;


	int n, len;
	fd_set rdfs;
	struct timeval timeout;
	char l[800];
	char locbuf[1024];
	int f;
	int i;
	int loop = 1;
	int first = 0;

	WriteDBGLog( "Starting to PROCESS input" );

	memset( gReadBuf, 0, sizeof( gReadBuf ) );
	gXMLBufferLen = 0;

	while( ( fgets( gReadBuf, sizeof( gReadBuf ), gFH ) ) > 0 )
	{
		rblen = strlen( gReadBuf );

		/* If Current buffer size + new Buffer being added is over the total buffer size BAD overflow */
		if ( (gXMLBufferLen + rblen) > gXMLMaxBufferLen )
		{
			WriteDBGLog( "Error BUFFER OVERFLOW" );
			WriteDBGLog( gXMLBuffer );
			WriteDBGLog( gReadBuf );
		}
		else
		{
			strcat( gXMLBuffer, gReadBuf );
			gXMLBufferLen += rblen;


			/* Check if this is the final XML package Ending */
			/* Since I'm not really parsing XML, I am assuming the Rainforest dongle is spitting out its specific XML */
			/* They always add two space for XML inbetween the start and stop So the closing XML will always be </    */
			if ( strncmp( gReadBuf, "</",  2 ) == 0 )
			{
				// Write to the Database
				WriteXML2DB( gSQLTable, gXMLBuffer );

				// Write to the LOG file if its turned on
				WriteDBGLog( gXMLBuffer );


				// Clear the XML buffer
				memset( gXMLBuffer, 0, sizeof( gXMLBuffer ) );
				gXMLBufferLen = 0;

				// This code will send a reset on the start of the program when it receives its first </ ending XML and so on
				if ( first == 0 )
				{
					SendCmd( "reset" );
					first++;
				}
				else if ( first == 1 )
				{
					SendCmd( "get_device_info" );
					first++;
				}
				else if ( first == 2 )
				{
					SendCmd( "get_network_info" );
					first++;
				}


			}
		}

		memset( gReadBuf, 0, sizeof( gReadBuf ) );
	}



	return ;
	

}