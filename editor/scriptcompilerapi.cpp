/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 


#include "stdafx.h"
#include "ScriptCompilerAPI.h"
#include "appdatabase.h"
#include "descent.h"
#include "cfile.h"
#include "manage.h"
#include "pserror.h"
#include "mem.h"
#include "ddio.h"
#include "osiris_dll.h"
#include <stdarg.h>

#ifdef NEWEDITOR
#include "../neweditor/globals.h"
#endif

int ScriptCompile(tCompilerInfo *ci)
{
	char Compiler_path[_MAX_PATH];
	int Warning_level = -1;
	int Debug_type = -1;
	int len = _MAX_PATH;

	if(!cfexist(ci->source_filename))
		return CERR_SOURCENOEXIST;

#ifndef NEWEDITOR
	//make sure there is a compiler defined
	if(Database->read("EditorCompiler",Compiler_path,&len)){
		if(!cfexist(Compiler_path)){			
			OutrageMessageBox("The configured virtual compiler (%s)\ncannot be found.  Please make sure\nyou have specified a proper path and\nthe file exists.  Go into the Script and\nLevel dialog to configure your compiler.",Compiler_path);
			return CERR_COMPILERMISSING;
		}
	}else{
		OutrageMessageBox("You haven't configured a virtual compiler to use.\nPlease go into the Script and Level Interface\nof the editor and select the configure button\nto configure a virtual compiler to use.\nFind Jeff for help on this.");
		return CERR_NOCOMPILERDEFINED;
	}

	Database->read_int("EditorVCWarningLevel",&Warning_level);
	Database->read_int("EditorVCDebugLevel",&Debug_type);
#else
	//New Editor setup

	// Determine the compiler
	CString cs_Compiler_Path = AfxGetApp()->GetProfileString("settings","EditorCompiler","");
	if(cs_Compiler_Path.GetLength()>1){
		if(!cfexist(cs_Compiler_Path.GetBuffer(0))){
			OutrageMessageBox("The configured virtual compiler (%s)\ncannot be found.  Please make sure\nyou have specified a proper path and\nthe file exists.  Select the configure button\nto configure your virtual compiler.",cs_Compiler_Path.GetBuffer(0));
			return CERR_COMPILERMISSING;
		}else
		{
			//valid compiler
			strcpy(Compiler_path,cs_Compiler_Path.GetBuffer(0));
		}
	}else{
		OutrageMessageBox("You haven't configured a virtual compiler to use.\nPlease select the configure option\nto configure a virtual compiler to use.");
		return CERR_NOCOMPILERDEFINED;
	}

	// Determine the Warning level
	Warning_level = AfxGetApp()->GetProfileInt("settings","EditorVCWarningLevel",3);	
	ASSERT(Warning_level>=0 && Warning_level<=4);
	if(Warning_level<0 || Warning_level>4) Warning_level = 3;

	// Determine the Debug type
	Debug_type = AfxGetApp()->GetProfileInt("settings","EditorVCDebugLevel",2);
	ASSERT(Debug_type>=0 && Debug_type<=2);
	if(Debug_type<0 || Debug_type>2) Debug_type = 2;

#endif

	SECURITY_ATTRIBUTES sa          = {0};
	STARTUPINFO         si          = {0};
	PROCESS_INFORMATION pi          = {0};
	HANDLE              hPipeOutputRead  = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead   = NULL;
	HANDLE              hPipeInputWrite  = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;
	CHAR                szBuffer[256];

	sa.nLength = sizeof(sa);      
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Create pipe for standard output redirection.
	CreatePipe(&hPipeOutputRead,  // read handle
               &hPipeOutputWrite, // write handle
               &sa,      // security attributes
               0      // number of bytes reserved for pipe - 0 default
               );      // Create pipe for standard input redirection.
	
	CreatePipe(&hPipeInputRead,  // read handle
		       &hPipeInputWrite, // write handle
               &sa,      // security attributes
               0      // number of bytes reserved for pipe - 0 default
               );

	// Make child process use hPipeOutputWrite as standard out,
	// and make sure it does not show on screen.      
	si.cb = sizeof(si);
	si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;      
	si.hStdInput   = hPipeInputRead;
	si.hStdOutput  = hPipeOutputWrite;      
	si.hStdError   = hPipeOutputWrite;

	CString cline;
	
	cline = Compiler_path;
	cline += " -f ";
	cline += "\"";
	cline += ci->source_filename;
	cline += "\"";
	cline += " -dir ";
	cline += LocalScriptDir;
	if(ci->script_type==ST_LEVEL) cline += " -level";
	switch(Warning_level){	
	case 0:
		cline += " -w 0";
		break;
	case 1:
		cline += " -w 1";
		break;
	case 2:
		cline += " -w 2";
		break;
	case 3:
		cline += " -w 3";
		break;
	case 4:
		cline += " -w 4";
		break;
	default:
		cline += " -w 3";
		break;
	}
	switch(Debug_type){
	case 1:
		cline += " -d coff";
		break;
	case 2:
		cline += " -d c7";
		break;
	}

	// Game structure checksum
#ifndef NEWEDITOR
	char temp_c[128];
	sprintf(temp_c," -checksum %lu",Osiris_game_checksum);
	cline += temp_c;
#endif

	CreateProcess (NULL,cline.GetBuffer(0),NULL,NULL,TRUE,0,NULL,NULL,&si,&pi);

	// Now that handles have been inherited, close it to be safe.
	// You don't want to read or write to them accidentally.
	CloseHandle(hPipeOutputWrite);      
	CloseHandle(hPipeInputRead);

	// Now test to capture DOS application output by reading
    // hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.
	CString temp;
	while(TRUE)      {
		bTest=ReadFile(
			hPipeOutputRead,      // handle of the read end of our pipe
            &szBuffer,            // address of buffer that receives data
            256,                  // number of bytes to read
            &dwNumberOfBytesRead, // address of number of bytes read
            NULL                  // non-overlapped.            
			);

		if (!bTest){
			break;        
		}

		// do something with data.
        szBuffer[dwNumberOfBytesRead] = 0;  // null terminate

		if(ci->callback){
			int new_line_count = 0;
			char *ptr = szBuffer;
			while ( *ptr ){
				if( *ptr=='\n' )
					new_line_count++;
				ptr++;
			}
			char *data,*d_ptr;
			d_ptr = data = (char *)mem_malloc(dwNumberOfBytesRead+1+new_line_count);
			if(data){
				ptr = (char *)szBuffer;

				while ( *ptr ){
					if( *ptr == '\n' ){
						*d_ptr = '\r'; d_ptr++;
						*d_ptr = '\n'; d_ptr++;
					}else if( *ptr != '\r'){
						*d_ptr = *ptr; d_ptr++;
					}
					ptr++;
				}
				*d_ptr = '\0';

				(*ci->callback)(data);
				mem_free(data);
			}
		}
	}

	// Wait for spawn to finish.
	WaitForSingleObject (pi.hProcess, INFINITE);

	// Close all remaining handles      
	CloseHandle (pi.hProcess);
	CloseHandle (hPipeOutputRead);      
	CloseHandle (hPipeInputWrite);

	return CERR_NOERR;
}

bool ScriptCreateEmptyLevelScript(char *filename);
bool ScriptCreateEmptyGameScript(char *filename);
bool ScriptCreateEmptyScript(char *filename,ubyte script_type)
{
	if(cfexist(filename)){
		char buffer[512];
		sprintf(buffer,"A script by the name of %s\nalready exists.",filename);
		AfxMessageBox(buffer,MB_OK);
		return false;
	}

	switch(script_type){
	case ST_GAME:
		return ScriptCreateEmptyGameScript(filename);
		break;
	case ST_LEVEL:
		return ScriptCreateEmptyLevelScript(filename);
		break;
	}

	return false;
}

#define VERSION "0.1"
#define O(x) outputtofile x
CFILE *CurrentFile;

void outputtofile(char *format, ...)
{
	char buffer[1024];
	va_list marker;
	va_start(marker,format);
	vsprintf(buffer,format,marker);
	cf_WriteString(CurrentFile,buffer);
	va_end(marker);
}

bool ScriptCreateEmptyLevelScript(char *filename)
{
	char fullpath[_MAX_PATH];
	ddio_MakePath(fullpath,LocalScriptDir,filename,NULL);
	CurrentFile = cfopen(fullpath,"wt");
	if(!CurrentFile)
		return false;

	O(("// %s",filename));
	O(("// %s",VERSION));
	O(("#include <stdio.h>"));
	O(("#include <stdlib.h>"));
	O(("#include <string.h>"));
	O(("#include \"osiris_import.h\""));
	O(("#include \"osiris_common.h\""));
	O((""));
	
	O(("#ifdef _MSC_VER		//Visual C++ Build"));
	O(("#define STDCALL		__stdcall"));
	O(("#define STDCALLPTR	*STDCALL"));
	O(("#else				//Non-Visual C++ Build"));
	O(("#define STDCALL __attribute__((stdcall))"));
	O(("#define STDCALLPTR	STDCALL*"));
	O(("#endif"));
	O((""));

	O(("#ifdef __cplusplus"));
	O(("extern \"C\"{"));
	O(("#endif"));
	O(("char	STDCALL InitializeDLL(tOSIRISModuleInit *func_list);"));
	O(("void	STDCALL ShutdownDLL(void);"));
	O(("int     STDCALL GetGOScriptID(char *name,ubyte is_door);"));
	O(("void	STDCALLPTR CreateInstance(int id);"));
	O(("void	STDCALL DestroyInstance(int id,void *ptr);"));
	O(("short	STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data);"));
	O(("int		STDCALL GetTriggerScriptID(int trigger_room, int trigger_face );"));
	O(("int		STDCALL GetCOScriptList( int **list, int **id_list );"));
	O(("int		STDCALL SaveRestoreState( void *file_ptr, ubyte saving_state );"));
	O(("#ifdef __cplusplus"));
	O(("}"));
	O(("#endif"));
	O((""));

	O(("int String_table_size = 0;"));
	O(("char **String_table = NULL;"));
	O(("static char *_Error_string = \"!!ERROR MISSING STRING!!\";"));
	O(("static char *_Empty_string = \"\";"));
	O(("char *GetStringFromTable(int index)"));
	O(("{"));
	O(("	if( (index<0) || (index>=String_table_size) )"));
	O(("		return _Error_string;"));
	O(("	if(!String_table[index])"));
	O(("		return _Empty_string;"));
	O(("		return String_table[index];"));
	O(("}"));
	O(("#define TXT(x) GetStringFromTable(x)"));

	O(("//	InitializeDLL"));
	O(("//	Purpose:"));
	O(("//		This function gets called when the DLL first gets loaded.  It will only be called once (until the"));
	O(("//	DLL is unloaded).  Passed in is a struct of data passed from the game needed for the DLL to interact"));
	O(("//	with D3.  Usually this function will just call osicommon_Initialize(), which sets up the imported"));
	O(("//	functions.  However, you can alloc some memory or whatever in this function, and free it in ShutdownDLL()."));
	O(("//	Note: You cannot call any imported functions until osicommon_Initialize() is called."));
	O(("//	Returns 1 if initialization went ok, 0 if there was an error and the DLL should not be loaded."));
	O(("char STDCALL InitializeDLL(tOSIRISModuleInit *func_list)"));
	O(("{"));
	O(("	osicommon_Initialize(func_list);"));
	O(("	String_table_size = func_list->string_count;"));
	O(("	String_table = func_list->string_table;"));
	O(("	if(func_list->game_checksum!=CHECKSUM)"));
	O(("	{ "));
	O(("		mprintf(0,\"Game-Checksum FAIL!!! (%%ul!=%%ul)\\n\",func_list->game_checksum,CHECKSUM);"));
	O(("		mprintf(0,\"RECOMPILE YOUR SCRIPTS!!!\\n\");"));
	O(("		return 0;"));
	O(("	}"));
	O(("	return 1;"));
	O(("}"));
	O((""));

	O(("//	ShutdownDLL"));
	O(("//	Purpose:"));
	O(("//		This function is called right before a DLL is about to be unloaded from memory.  You can free"));
	O(("//	any unfree'd memory, or anything else you need to do.  Don't worry about destroying any instances"));
	O(("//	of scripts, as they will all be automatically destroyed before this function is called.  The"));
	O(("//	same goes for any memory allocated with Scrpt_MemAlloc(), as this will automatically be freed"));
	O(("//	when a scripts instance is destroyed."));
	O(("void STDCALL ShutdownDLL(void)"));
	O(("{"));
	O(("}"));
	O((""));

	O(("//	GetGOScriptID"));
	O(("//	Purpose:"));
	O(("//		Given the name of the object (from it's pagename), this function will search through it's"));
	O(("//	list of General Object Scripts for a script with a matching name (to see if there is a script"));
	O(("//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID"));
	O(("//	is to be returned back to Descent 3.  This ID will be used from here on out for all future"));
	O(("//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible"));
	O(("//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER"));
	O(("//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a"));
	O(("//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script"));
	O(("//	does not exist in the DLL."));
	O(("//	The only reserved ID is 0, which must be used for the level script"));
	O(("int STDCALL GetGOScriptID(char *name,ubyte isdoor)"));
	O(("{"));
	O(("	return -1;"));
	O(("}"));
	O((""));

	O(("//	CreateInstance"));
	O(("//	Purpose:"));
	O(("//		Given an ID from a call to GetGOScriptID(), GetTriggerScriptID() or GetCOScriptList(), this"));
	O(("//	function will create a new instance for that particular script (by allocating and initializing"));
	O(("//	memory, etc.).  A pointer to this instance is to be returned back to Descent 3.  This pointer will"));
	O(("//	be passed around, along with the ID for CallInstanceEvent() and DestroyInstance().  Return NULL"));
	O(("//	if there was an error."));
	O(("//	The only reserved ID is 0, which must be used for the level script"));
	O(("void STDCALLPTR CreateInstance(int id)"));
	O(("{"));
	O(("	return NULL;"));
	O(("}"));
	O((""));

	O(("//	DestroyInstance"));
	O(("//	Purpose:"));
	O(("//		Given an ID, and a pointer to a particular instance of a script, this function will delete and"));
	O(("//	destruct all information associated with that script, so it will no longer exist."));
	O(("void STDCALL DestroyInstance(int id,void *ptr)"));
	O(("{"));
	O(("}"));
	O((""));

	O(("//	CallInstanceEvent"));
	O(("//	Purpose:"));
	O(("//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of"));
	O(("//	information about the event, this function will translate who this event belongs to and"));
	O(("//	passes the event to that instance of the script to be handled.  Return a combination of"));
	O(("//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the"));
	O(("//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level"));
	O(("//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,"));
	O(("//	than the chain is broken and those scripts of lower priority will never get the event.  Return"));
	O(("//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into"));
	O(("//	the game for that event.  This only pertains to certain events.  If the chain continues"));
	O(("//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority"));
	O(("//	scripts return value."));
	O(("//	The only reserved ID is 0, which must be used for the level script"));
	O(("short STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data)"));
	O(("{"));
	O(("	return CONTINUE_CHAIN|CONTINUE_DEFAULT;"));
	O(("}"));
	O((""));

	O(("//	GetTriggerScriptID"));
	O(("//	Purpose:"));
	O(("//		Given a room and face number, this function will return a unique ID (global DLL unique)"));
	O(("//	in which Descent 3 should use for all future interaction will the DLL when refering to"));
	O(("//	this trigger.  Return -1 if there is no trigger script available for the specified trigger."));
	O(("//	The only reserved ID is 0, which must be used for the level script"));
	O(("int	STDCALL GetTriggerScriptID(int trigger_room,int trigger_face)"));
	O(("{"));
	O(("	return -1;"));
	O(("}"));
	O((""));

	O(("//	GetCOScriptList"));
	O(("//	Purpose:"));
	O(("//		This function returns the pointers to the 2 arrays that Descent 3 should use to determine"));
	O(("//	what object's have custom scripts in this level DLL.  list should be set to point to the array"));
	O(("//	of object handles that have custom scripts in this DLL.  id_list should be set to point to the"));
	O(("//	corresponding array of unique IDs that match with the object handle list. This function should"));
	O(("//	return the size of the arrays returned...if no custom scripts are available, than this function"));
	O(("//	returns 0."));
	O(("//	The only reserved ID is 0, which must be used for the level script"));
	O(("int	STDCALL GetCOScriptList( int **list, int **id_list )"));
	O(("{"));
	O(("	return 0;"));
	O(("}"));

	O(("//	SaveRestoreState"));
	O(("//	Purpose:"));
	O(("//		This function is called when Descent 3 is saving or restoring the game state.  In this function"));
	O(("//	you should save/restore any global data that you want preserved through load/save (which includes"));
	O(("//	demos).  You must be very careful with this function, corrupting the file (reading or writing too"));
	O(("//	much or too little) may be hazardous to the game (possibly making it impossible to restore the"));
	O(("//	state).  It would be best to use version information to keep older versions of saved states still"));
	O(("//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE"));
	O(("//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should"));
	O(("//	write data to the file_ptr, 0 when you should read in the data."));
	O(("int STDCALL SaveRestoreState( void *file_ptr, ubyte saving_state )"));
	O(("{"));
	O(("	return 0;"));
	O(("}"));
	O((""));
	
	cfclose(CurrentFile);
	return true;
}

bool ScriptCreateEmptyGameScript(char *filename)
{
	char fullpath[_MAX_PATH];
	ddio_MakePath(fullpath,LocalScriptDir,filename,NULL);
	CurrentFile = cfopen(fullpath,"wt");
	if(!CurrentFile)
		return false;

	O(("// %s",filename));
	O(("// %s",VERSION));
	O(("#include <stdio.h>"));
	O(("#include <stdlib.h>"));
	O(("#include <string.h>"));
	O(("#include \"osiris_import.h\""));
	O(("#include \"osiris_common.h\""));
	O((""));
	
	O(("#ifdef _MSC_VER		//Visual C++ Build"));
	O(("#define STDCALL		__stdcall"));
	O(("#define STDCALLPTR	*STDCALL"));
	O(("#else				//Non-Visual C++ Build"));
	O(("#define STDCALL __attribute__((stdcall))"));
	O(("#define STDCALLPTR	STDCALL*"));
	O(("#endif"));
	O((""));

	O(("#ifdef __cplusplus"));
	O(("extern \"C\"{"));
	O(("#endif"));
	O(("char	STDCALL InitializeDLL(tOSIRISModuleInit *func_list);"));
	O(("void	STDCALL ShutdownDLL(void);"));
	O(("int     STDCALL GetGOScriptID(char *name,ubyte isdoor);"));
	O(("void	STDCALLPTR CreateInstance(int id);"));
	O(("void	STDCALL DestroyInstance(int id,void *ptr);"));
	O(("short	STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data);"));
	O(("int		STDCALL SaveRestoreState( void *file_ptr, ubyte saving_state );"));
	O(("#ifdef __cplusplus"));
	O(("}"));
	O(("#endif"));
	O((""));

	O(("int String_table_size = 0;"));
	O(("char **String_table = NULL;"));
	O(("static char *_Error_string = \"!!ERROR MISSING STRING!!\";"));
	O(("static char *_Empty_string = \"\";"));
	O(("char *GetStringFromTable(int index)"));
	O(("{"));
	O(("	if( (index<0) || (index>=String_table_size) )"));
	O(("		return _Error_string;"));
	O(("	if(!String_table[index])"));
	O(("		return _Empty_string;"));
	O(("		return String_table[index];"));
	O(("}"));
	O(("#define TXT(x) GetStringFromTable(x)"));

	O(("//	InitializeDLL"));
	O(("//	Purpose:"));
	O(("//		This function gets called when the DLL first gets loaded.  It will only be called once (until the"));
	O(("//	DLL is unloaded).  Passed in is a struct of data passed from the game needed for the DLL to interact"));
	O(("//	with D3.  Usually this function will just call osicommon_Initialize(), which sets up the imported"));
	O(("//	functions.  However, you can alloc some memory or whatever in this function, and free it in ShutdownDLL()."));
	O(("//	Note: You cannot call any imported functions until osicommon_Initialize() is called."));
	O(("//	Returns 1 if initialization went ok, 0 if there was an error and the DLL should not be loaded."));
	O(("char STDCALL InitializeDLL(tOSIRISModuleInit *func_list)"));
	O(("{"));
	O(("	osicommon_Initialize((tOSIRISModuleInit *)func_list);"));
	O(("	String_table_size = func_list->string_count;"));
	O(("	String_table = func_list->string_table;"));
	O(("	if(func_list->game_checksum!=CHECKSUM)"));
	O(("	{ "));
	O(("		mprintf(0,\"Game-Checksum FAIL!!! (%%ul!=%%ul)\\n\",func_list->game_checksum,CHECKSUM);"));
	O(("		mprintf(0,\"RECOMPILE YOUR SCRIPTS!!!\\n\");"));
	O(("		return 0;"));
	O(("	}"));
	O(("	return 1;"));
	O(("}"));
	O((""));

	O(("//	ShutdownDLL"));
	O(("//	Purpose:"));
	O(("//		This function is called right before a DLL is about to be unloaded from memory.  You can free"));
	O(("//	any unfree'd memory, or anything else you need to do.  Don't worry about destroying any instances"));
	O(("//	of scripts, as they will all be automatically destroyed before this function is called.  The"));
	O(("//	same goes for any memory allocated with Scrpt_MemAlloc(), as this will automatically be freed"));
	O(("//	when a scripts instance is destroyed."));
	O(("void STDCALL ShutdownDLL(void)"));
	O(("{"));
	O(("}"));
	O((""));

	O(("//	GetGOScriptID"));
	O(("//	Purpose:"));
	O(("//		Given the name of the object (from it's pagename), this function will search through it's"));
	O(("//	list of General Object Scripts for a script with a matching name (to see if there is a script"));
	O(("//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID"));
	O(("//	is to be returned back to Descent 3.  This ID will be used from here on out for all future"));
	O(("//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible"));
	O(("//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER"));
	O(("//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a"));
	O(("//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script"));
	O(("//	does not exist in the DLL."));
	O(("int STDCALL GetGOScriptID(char *name,ubyte isdoor)"));
	O(("{"));
	O(("	return -1;"));
	O(("}"));
	O((""));

	O(("//	CreateInstance"));
	O(("//	Purpose:"));
	O(("//		Given an ID from a call to GetGOScriptID(), this function will create a new instance for that"));
	O(("//	particular script (by allocating and initializing memory, etc.).  A pointer to this instance"));
	O(("//	is to be returned back to Descent 3.  This pointer will be passed around, along with the ID"));
	O(("//	for CallInstanceEvent() and DestroyInstance().  Return NULL if there was an error."));
	O(("void STDCALLPTR CreateInstance(int id)"));
	O(("{"));
	O(("	return NULL;"));
	O(("}"));
	O((""));

	O(("//	DestroyInstance"));
	O(("//	Purpose:"));
	O(("//		Given an ID, and a pointer to a particular instance of a script, this function will delete and"));
	O(("//	destruct all information associated with that script, so it will no longer exist."));
	O(("void STDCALL DestroyInstance(int id,void *ptr)"));
	O(("{"));
	O(("}"));
	O((""));

	O(("//	CallInstanceEvent"));
	O(("//	Purpose:"));
	O(("//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of"));
	O(("//	information about the event, this function will translate who this event belongs to and"));
	O(("//	passes the event to that instance of the script to be handled.  Return a combination of"));
	O(("//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the"));
	O(("//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level"));
	O(("//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,"));
	O(("//	than the chain is broken and those scripts of lower priority will never get the event.  Return"));
	O(("//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into"));
	O(("//	the game for that event.  This only pertains to certain events.  If the chain continues"));
	O(("//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority"));
	O(("//	scripts return value."));
	O(("short STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data)"));
	O(("{"));
	O(("	return CONTINUE_CHAIN|CONTINUE_DEFAULT;"));
	O(("}"));
	O((""));

	O(("//	SaveRestoreState"));
	O(("//	Purpose:"));
	O(("//		This function is called when Descent 3 is saving or restoring the game state.  In this function"));
	O(("//	you should save/restore any global data that you want preserved through load/save (which includes"));
	O(("//	demos).  You must be very careful with this function, corrupting the file (reading or writing too"));
	O(("//	much or too little) may be hazardous to the game (possibly making it impossible to restore the"));
	O(("//	state).  It would be best to use version information to keep older versions of saved states still"));
	O(("//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE"));
	O(("//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should"));
	O(("//	write data to the file_ptr, 0 when you should read in the data."));
	O(("int STDCALL SaveRestoreState( void *file_ptr, ubyte saving_state )"));
	O(("{"));
	O(("	return 0;"));
	O(("}"));
	O((""));

	cfclose(CurrentFile);
	return true;
}
