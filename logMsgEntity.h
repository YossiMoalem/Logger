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
//TODO: changed the bellow to formulla...
#define MAX_MSG_SIZE  400

class logMsgEntity
{
	enum msgState
	{
		MS_Unused 	= 0,
		MS_InProgress 	= 1,
		MS_Ready	= 2,
		MS_BeingFlushed = 3,
		MS_Flushed	= 4
	};

	private :

	msgState 			m_state;
	char 				m_msgText [ MAX_MSG_TEXT_SIZE+ 1 ];
	char 				m_funcName [ MAX_FUNC_NAME_SIZE + 1 ];
	time_t 				m_time;
	pid_t				m_tid;
	int 				m_severity;

	public :

	logMsgEntity() : m_state (MS_Unused)
	{
		m_msgText[MAX_MSG_TEXT_SIZE] = 0;
		m_funcName[MAX_FUNC_NAME_SIZE] = 0;
	}

	int set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity)
	{
		//TODO:
		// sstate -> atomic
		// Validate the state!
		// Most Important state validation : not MS_BeingFlushed..
		// What to do in such case????
		m_state = MS_InProgress; 
		strncpy (m_msgText, i_pNewMsg, MAX_MSG_TEXT_SIZE);
		strncpy (m_funcName, i_pFuncName, MAX_FUNC_NAME_SIZE);
		m_time = i_time;
		m_time = i_tid;
		m_state = MS_Ready;
		m_severity = i_severity;
		return 0;
	}

	int get (logMsgFormatterWriter* i_logMsgFormatterWriter )
	{
		switch (m_state)
		{
			case MS_Unused:
				//We never wrote here. Do nothing.
				break;
			case MS_InProgress:
				//TODO : atomic
				m_state = MS_Flushed;
				i_logMsgFormatterWriter->write ("Massage is missing here ", "Internal Logger Error", 0, -1, 0);
				break;
			case MS_Ready:
				//TODO: check format
				//TODO: change severity to String!!
				//TODO : atomic
				m_state = MS_BeingFlushed;
				i_logMsgFormatterWriter->write ("m_msgText", m_funcName, m_time, m_tid, m_severity);
				m_state = MS_Flushed;
				break;
			case MS_Flushed:
				//Already flushed this message. Nothing to do ....
				break;
			case MS_BeingFlushed:
				//TODO: What to do here??????
				//Maybe do flash it, and mark it as flused, so when we finish the first flush, we'll know we had this message twice???
				break;
			default:
				assert (0);
		}
		return 0;
	}
};

#endif
