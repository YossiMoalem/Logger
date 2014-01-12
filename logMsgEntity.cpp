#include "logMsgEntity.h"
#include "loggerStatistics.h"

	logMsgEntity::logMsgEntity() : m_state (MS_Unused),m_lifeID (0)
	{
		m_msgText[MAX_MSG_TEXT_SIZE] = 0;
		m_funcName[MAX_FUNC_NAME_SIZE] = 0;
        
	}
//==================================================================================================================================================
    int logMsgEntity::set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID)
	{
		
        int retval =0;
             msgState oldMsgState= m_state;
        switch (m_state)
        {
           case MS_Unused:
           case MS_Ready:
           case MS_Flushed:
              m_state = MS_InProgress; 
              if (m_lifeID >= i_lifeID)
              {
              m_state = oldMsgState;
              retval = 0;
              }
              else
              {
                 strncpy (m_msgText, i_pNewMsg, MAX_MSG_TEXT_SIZE);
                 strncpy (m_funcName, i_pFuncName, MAX_FUNC_NAME_SIZE);
                 m_time = i_time;
                 m_tid = i_tid;
                 m_severity = i_severity;
                 m_lifeID = i_lifeID;
                 m_state = MS_Ready;
              }
              break;
           case MS_InProgress:
              loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_setWhileInProgress);
              //TODO : Need to handle 
              retval  = -1;
              break;
           case MS_BeingFlushed:
              loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_setWhileBeingFlushed);
              //TODO: What to do here??????
              retval  = -1;
             break;
           default:
              assert (0);
              retval = -1;
        }
        return retval;
	}

//==================================================================================================================================================
	int logMsgEntity::write (logMsgFormatterWriter* i_logMsgFormatterWriter,unsigned int i_expectedLifeID )
	{
		switch (m_state)
		{
			case MS_Unused:
				//We never wrote here. Do nothing.
				//i_logMsgFormatterWriter->write ("Massage is missing here ", "Unused", 0, -1, 0);
				break;
			case MS_InProgress:
				//TODO : do we really want this? if not also chamge if (m_state != MS_InProgress || m_state != MS_Flushed)
                //Notice, it cound be that "our" msg is being overwritten, and we have nothing to do now
                //BUT it coulld also be that our msg is just being writen, so we should wait for it (?) ...
				m_state = MS_Flushed;
				i_logMsgFormatterWriter->write ("Massage is missing here ", "Internal Logger Error", 0, -1, 0);
               return -1;
			case MS_Ready:
				m_state = MS_BeingFlushed;
                if (m_lifeID != i_expectedLifeID)
                {
                   m_state = MS_Ready;
                   return -1;
                }   
				i_logMsgFormatterWriter->write (m_msgText, m_funcName, m_time, m_tid, m_severity);
				m_state = MS_Flushed;
				break;
			case MS_BeingFlushed:
                loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_writeWhileBeingFlushed);
				//TODO: What to do here??????
				//Maybe do flash it, and mark it as flused, so when we finish the first flush, we'll know we had this message twice???
               return -1;
			case MS_Flushed:
				//Already flushed this message. Nothing to do ....
				break;
			default:
				assert (0);
		}
		return 0;
	}

