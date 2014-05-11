#include "logMsgEntity.h"
#include "loggerStatistics.h"

#ifndef NOLOCK
  #define LOCK_BEFORE_SET           pthread_mutex_lock(&m_lock);
  #define LOCK_BEFORE_WRITE         pthread_mutex_lock(&m_lock);
  #define UNLOCK_AFTER_SET          pthread_mutex_unlock(&m_lock);
  #define UNLOCK_AFTER_WRITE        pthread_mutex_unlock(&m_lock);
  #define INIT_LOCK                 pthread_mutexattr_t attr;\
                                       pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); \
                                       pthread_mutex_init(&m_lock,&attr);


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
   logMsgEntity::resultStatus  logMsgEntity::set (const char* i_pNewMsg, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,unsigned int i_lifeID,bool i_writeStack)
{

   resultStatus retval = RS_Unset;
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
               retval = RS_Success;
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
               retval = RS_Success;   
            } //else
         } //Lock block
         UNLOCK_AFTER_SET
            break;
      case MS_InProgress:
         loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_setWhileInProgress);
         retval  = RS_MsgBusy;
         break;
      case MS_BeingFlushed:
         loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_setWhileBeingFlushed);
         retval  = RS_MsgBusy;
         break;
      default:
         assert (0);
         retval = RS_InternalError;
   }
   return retval;
}

//==================================================================================================================================================
	logMsgEntity::resultStatus logMsgEntity::write (logMsgFormatterWriter* i_logMsgFormatterWriter,unsigned int i_expectedLifeID )
{

   resultStatus retval = RS_Unset;
   LOCK_BEFORE_WRITE ;
   if (m_lifeID > i_expectedLifeID)
   {
      retval =  RS_MsgOverWritten;
   } else if (m_lifeID < i_expectedLifeID)
   {
      retval =  RS_MsgNotYetWriten;
   } 
   else
   {   

      switch (m_state)
      {
         case MS_Unused:
            //We never wrote here. Do nothing.
            //i_logMsgFormatterWriter->write ("Massage is missing here ", "Unused", 0, -1, 0);
			retval = RS_Success;
            break;
         case MS_InProgress:
            //TODO : do we really want this? if not also change if (m_state != MS_InProgress || m_state != MS_Flushed)
            //Notice, it cound be that "our" msg is being overwritten, and we have nothing to do now
            //BUT it coulld also be that our msg is just being written, so we should wait for it (?) ...
            m_state = MS_Flushed;
            i_logMsgFormatterWriter->write ("Massage is missing here ", "Internal Logger Error", 0, -1, 0);
            retval =  RS_MsgBusy;
         case MS_Ready:
            m_state = MS_BeingFlushed;
            if (0== m_stack[0])
            {
               i_logMsgFormatterWriter->write (m_msgText, m_funcName, m_time, m_tid, m_severity);
            }else {
               Stackwalker::stackFrameBuff stackSymbol[STACK_SIZE];
               Stackwalker::stackSymbols(STACK_SIZE,m_stack,stackSymbol);
               i_logMsgFormatterWriter->writeWithStack (m_msgText, m_funcName, m_time, m_tid, m_severity,stackSymbol,STACK_SIZE);
            }
            m_state = MS_Flushed;
            break;
         case MS_BeingFlushed:
            loggerStatistics::instance()->inc_counter(loggerStatistics::logMsgEntity_writeWhileBeingFlushed);
            //TODO: What to do here??????
            //Maybe do flash it, and mark it as flused, so when we finish the first flush, we'll know we had this message twice???
            //Can we really be here?? there is only one reader, so only single thread put msg. in being flushed...
            retval =  RS_MsgBusy;
         case MS_Flushed:
            //Already flushed this message. Nothing to do ....
            break;
         default:
            assert (0);
            retval = RS_InternalError;
      }
   }

   UNLOCK_AFTER_WRITE ;
   return retval;
}

