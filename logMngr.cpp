#include "logMngr.h"
#include "outputHandler.h"
#include "utils.h"

//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);

//==================================================================================================================================================
logMngr::logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter ) : 
   m_flushSeverity(i_flushSeverity),
   m_pLogMsgFormatterWriter(i_pLogMsgFormatterWriter),
   m_msgTokenMngr(NUM_OF_LOG_MSGS)
{
   loggerStatistics::create();

   pthread_mutexattr_t startIndexMutexAttr;
   pthread_mutexattr_settype(&startIndexMutexAttr, PTHREAD_MUTEX_ERRORCHECK); //(PTHREAD_MUTEX_RECURSIVE for prod??)
   pthread_mutex_init(&m_startIndexMutex, &startIndexMutexAttr);

   sem_init (&m_shutDownSem, 0, 1);
   sem_wait(&m_shutDownSem);

   pthread_t                   outputWriterThread;
   pthread_attr_t attr;
   
   //TODO check retval
   int s = pthread_attr_init(&attr);
   s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   pthread_create(&outputWriterThread,&attr,&outputHandler::startOutputWriterThread,this);

}

//==================================================================================================================================================
logMngr::~logMngr()
{
   pthread_mutex_destroy(&m_startIndexMutex);
}

//==================================================================================================================================================
int logMngr::flushMessages (msgTokenMngr::msg_token_t i_entryIdentifier)
{

   pthread_mutex_lock(&m_startIndexMutex);

#if DEBUG >= 2
   PRINT_DEBUG ("Calling flush for identifier "<< i_entryIdentifier);
#endif

   m_queueFlushStartIndex.push(i_entryIdentifier);
   pthread_mutex_unlock(&m_startIndexMutex);
   return 0;
}


//==================================================================================================================================================
int logMngr::write (const char *const  i_pMsgText, const char *const  i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,bool i_writeStack)
{
   unsigned int curIndex = 0;
   unsigned int curLifeID = 0;
  msgTokenMngr::msg_token_t entryIdentifier = 0;
   int retval = 0;

   m_msgTokenMngr.getNextIndex(curIndex,curLifeID,entryIdentifier);

 if (i_severity >= m_flushSeverity)
   {
      retval = this->flushMessages(entryIdentifier);
   }


#if DEBUG >= 1
 PRINT_DEBUG(entryIdentifier
       << "|Severity: " <<i_severity 
       << " Time:  "     <<i_time 
       << " ThreadID: "   <<i_tid 
       << " FuncName: "   << i_pFuncName 
       << " Message: "   <<i_pMsgText);
#endif 
  logMsgEntity::resultStatus res = logMsgEntity::RS_Unset;
  do {
       res = m_msgs[curIndex].set(i_pMsgText,i_pFuncName,i_time,i_tid,i_severity,curLifeID,i_writeStack);
     //TODO need to handle/yield in case od retry,....
  } while(res != logMsgEntity::RS_Success );
   return retval;
}


//==================================================================================================================================================
void logMngr::startBlock()
{
   m_pLogMsgFormatterWriter->startBlock();
}
 
//==================================================================================================================================================
void logMngr::writeError (const char* i_pErrorMessage)
{
   m_pLogMsgFormatterWriter->writeError(i_pErrorMessage);
}

//==================================================================================================================================================
void logMngr::shutDown()
{
   
   pthread_mutex_lock(&m_startIndexMutex);

#if DEBUG >= 2
   PRINT_DEBUG ("******Calling shutDown *******");
#endif

   m_queueFlushStartIndex.push(SHUTDOWN_ENTRY);
   pthread_mutex_unlock(&m_startIndexMutex);
   //Wait for the writer to finish.
   sem_wait(&m_shutDownSem);

   loggerStatistics::instance()->print();
}
