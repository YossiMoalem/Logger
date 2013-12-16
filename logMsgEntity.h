#ifndef LOG_MSG_ENTITY
#define LOG_MSG_ENTITY

#include <sys/types.h>  // for pid_t
#include <time.h> //for time_t
#include <string.h>//for strncpy
#include <stdio.h> //for sprintf
#include <assert.h> //for assert...


#include "logMsgFormatterWriter.h"

#define MAX_MSG_TEXT_SIZE  200
#define MAX_FUNC_NAME_SIZE 100

class logMsgEntity
{
	enum msgState
	{
		MS_Unused 	    = 0,
		MS_InProgress 	= 1,
		MS_Ready	    = 2,
		MS_BeingFlushed = 3,
		MS_Flushed	    = 4
	};

	public :

    logMsgEntity();

    int set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID);
	int write (logMsgFormatterWriter* i_logMsgFormatterWriter ,unsigned int i_expectedLifeID);
    unsigned int getLifeID() volatile { return m_lifeID; }
    
    private :

	msgState 			m_state;
	char 				m_msgText [ MAX_MSG_TEXT_SIZE+ 1 ];
	char 				m_funcName [ MAX_FUNC_NAME_SIZE + 1 ];
	time_t 				m_time;
	pid_t				m_tid;
	int 				m_severity;
    unsigned int        m_lifeID;
};

#endif
