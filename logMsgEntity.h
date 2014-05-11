#ifndef LOG_MSG_ENTITY
#define LOG_MSG_ENTITY

#include <sys/types.h>  // for pid_t
#include <time.h> //for time_t
#include <string.h>//for strncpy
#include <stdio.h> //for sprintf
#include <assert.h> //for assert...
#include <pthread.h>//for the mutex

#include "logMsgFormatterWriter.h"
#include "stackwalker.h"


#define MAX_MSG_TEXT_SIZE  200
#define MAX_FUNC_NAME_SIZE 100
#define STACK_SIZE 5

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
   public:

	enum resultStatus
    {
		RS_Unset 			= 0,
       RS_Success            = 1,
       RS_MsgOverWritten    = 2,
       RS_MsgNotYetWriten   = 3,
       RS_MsgBusy           = 4,
       RS_InternalError     = 5,
       RS_Last
    };

    logMsgEntity();

    resultStatus set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID,bool i_writeStack);
	resultStatus  write (logMsgFormatterWriter* i_logMsgFormatterWriter ,unsigned int i_expectedLifeID);
    
    private :

	msgState 			m_state;
	char 				m_msgText [ MAX_MSG_TEXT_SIZE+ 1 ];
	char 				m_funcName [ MAX_FUNC_NAME_SIZE + 1 ];
	time_t 				m_time;
	pid_t				m_tid;
	int 				m_severity;
    unsigned int        m_lifeID;
   Stackwalker::stackFrameAddr m_stack[STACK_SIZE];

#ifndef NOLOCK
    pthread_mutex_t     m_lock;
#endif    
};

#endif
