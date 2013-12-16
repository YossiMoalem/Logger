#include "logMngr.h"


//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);

//==================================================================================================================================================
logMngr::logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter ) : 
   m_flushSeverity(i_flushSeverity),
   m_pLogMsgFormatterWriter(i_pLogMsgFormatterWriter),
   m_entityIdentifierType(NUM_OF_LOG_MSGS)
{
   pthread_mutex_init(&m_startIndexMutex, NULL);
   pthread_mutex_init(&m_shutDownMutex, NULL);
   pthread_mutex_lock(&m_shutDownMutex);

   pthread_t                   outputWriterThread;
   pthread_attr_t attr;
   
   //TODO check retval
   int s = pthread_attr_init(&attr);
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   pthread_create(&outputWriterThread,&attr,&startOutputWriterThread,this);

}

//==================================================================================================================================================
logMngr::~logMngr()
{
   pthread_mutex_destroy(&m_startIndexMutex);
}

//==================================================================================================================================================
int logMngr::flushMessages (entityIdentifierType::entity_identifier_t i_entryIdentifier)
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
int logMngr::write (const char *const  i_pMsgText, const char *const  i_pFuncName, time_t i_time, pid_t i_tid, int i_severity)
{
   unsigned int curIndex = 0;
   unsigned int curLifeID = 0;
  entityIdentifierType::entity_identifier_t entryIdentifier = 0;
   int retval = 0;

   m_entityIdentifierType.getNextIndex(curIndex,curLifeID,entryIdentifier);

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
       << " Message: "   <<i_pMsgText <<std::endl);
#endif 
  while(0 != m_msgs[curIndex].set(i_pMsgText,i_pFuncName,i_time,i_tid,i_severity,curLifeID))
  {
     //TODO need to handle/yield....
  }

   return retval;
}


//==================================================================================================================================================
void * logMngr::startOutputWriterThread (void * i_logMngr)
{
   logMngr *currLogMngr = static_cast <logMngr*> (i_logMngr);
   outputHandler *pOutputHandler = new outputHandler(*currLogMngr);
   pOutputHandler->doSomething();
   return NULL;
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
   pthread_mutex_lock(&m_shutDownMutex);
}
