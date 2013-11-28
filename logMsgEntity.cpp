#include "logMsgEntity.h"

	logMsgEntity::logMsgEntity() : m_state (MS_Unused),m_lifeID (0)
	{
		m_msgText[MAX_MSG_TEXT_SIZE] = 0;
		m_funcName[MAX_FUNC_NAME_SIZE] = 0;
        
	}
//==================================================================================================================================================
    int logMsgEntity::set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID)
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
		m_tid = i_tid;
		m_state = MS_Ready;
		m_severity = i_severity;
		m_lifeID=i_lifeID;
        return 0;
	}

//==================================================================================================================================================
	int logMsgEntity::write (logMsgFormatterWriter* i_logMsgFormatterWriter )
	{
		switch (m_state)
		{
			case MS_Unused:
				//We never wrote here. Do nothing.
				//i_logMsgFormatterWriter->write ("Massage is missing here ", "Unused", 0, -1, 0);
				break;
			case MS_InProgress:
				//TODO : atomic
				m_state = MS_Flushed;
				i_logMsgFormatterWriter->write ("Massage is missing here ", "Internal Logger Error", 0, -1, 0);
				break;
			case MS_Ready:
				//TODO: check format
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

