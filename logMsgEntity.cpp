#include "logMsgEntity.h"
#include "loggerStatistics.h"

#ifndef NOLOCK
  #define LOCK_BEFORE_SET           pthread_mutex_lock(&m_lock);
  #define LOCK_BEFORE_WRITE         pthread_mutex_lock(&m_lock);
  #define UNLOCK_AFTER_SET          pthread_mutex_unlock(&m_lock);
  #define UNLOCK_AFTER_WRITE        pthread_mutex_unlock(&m_lock);
  #define INIT_LOCK                 pthread_mutex_init(&m_lock,NULL);

#else
   #warning You are compiling with NO LOCK log messages can get cprrupted!!!
   #define LOCK_BEFORE_SET     
   #define LOCK_BEFORE_WRITE   
   #define UNLOCK_AFTER_SET    
   #define UNLOCK_AFTER_WRITE  
   #define INIT_LOCK                 
#endif


	logMsgEntity::logMsgEntity() : m_state (MS_Unused),m_lifeID (0)
	{
		m_msgText[MAX_MSG_TEXT_SIZE] = 0;
		m_funcName[MAX_FUNC_NAME_SIZE] = 0;
                INIT_LOCK
	}
//==================================================================================================================================================
    int logMsgEntity::set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID,bool i_writeStack)
{

   int retval =0;
   switch (m_state)
   {
      case MS_Unused:
      case MS_Ready:
      case MS_Flushed:

         LOCK_BEFORE_SET
         {  
            msgState oldMsgState = m_state;
            m_state = MS_InProgress; 
            if (m_lifeID >= i_lifeID)
            {
               //We got an old message. No need to keep it.
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
               if (true == i_writeStack)
               {
                  Stackwalker::getStacktrace(STACK_SIZE,m_stack,1);                 
               } else {
                  m_stack[0]= 0;
               }
                  
            }
         }
         UNLOCK_AFTER_SET
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
				//TODO : do we really want this? if not also change if (m_state != MS_InProgress || m_state != MS_Flushed)
                //Notice, it cound be that "our" msg is being overwritten, and we have nothing to do now
                //BUT it coulld also be that our msg is just being written, so we should wait for it (?) ...
				m_state = MS_Flushed;
				i_logMsgFormatterWriter->write ("Massage is missing here ", "Internal Logger Error", 0, -1, 0);
               return -1;
			case MS_Ready:
				m_state = MS_BeingFlushed;
                if (m_lifeID != i_expectedLifeID)
                {
                   //The message is out os seq (too old, or to new). 
                   //TODO: (think)  if it is too new - "our one" was overwritten, nothing to do
                   //if it is too old, the one we are waiting for, was not written yet, maybe wait for it
                   //(may cause ther messages to be overwritten while we are wating...)
                   m_state = MS_Ready;
                   return -1;
                }   
				LOCK_BEFORE_WRITE
                if (0== m_stack[0])
                {
                   i_logMsgFormatterWriter->write (m_msgText, m_funcName, m_time, m_tid, m_severity);
                }else {
                   Stackwalker::stackFrameBuff stackSymbol[STACK_SIZE];
                   Stackwalker::stackSymbols(STACK_SIZE,m_stack,stackSymbol);
                   i_logMsgFormatterWriter->writeWithStack (m_msgText, m_funcName, m_time, m_tid, m_severity,stackSymbol,STACK_SIZE);
                }
                m_state = MS_Flushed;
				UNLOCK_AFTER_WRITE
				break;
			case MS_BeingFlushed:
                loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_writeWhileBeingFlushed);
				//TODO: What to do here??????
				//Maybe do flash it, and mark it as flused, so when we finish the first flush, we'll know we had this message twice???
                //Can we really be here?? there is only one reader, so only single thread put msg. in being flushed...
               return -1;
			case MS_Flushed:
				//Already flushed this message. Nothing to do ....
				break;
			default:
				assert (0);
		}
		return 0;
	}

