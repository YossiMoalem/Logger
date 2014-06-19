#include "outputHandler.h"
#include "utils.h"

#include <pthread.h> 


//============================================================================
template <class Writer>
logMngr<Writer>::logMngr(int i_flushSeverity) : 
   m_flushSeverity(i_flushSeverity),
   m_msgTokenMngr(NUM_OF_LOG_MSGS),
   m_pOutputHandler(new outputHandler<Writer>(m_msgs, m_flushTokenHolder))
{
   loggerStatistics::instance(); //Just in order to create the statistics mngr....


   pthread_t                   outputWriterThread;
   pthread_attr_t attr;
   
   pthread_attr_init(&attr); //Always succeeds on Linux
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//detachstate is fine, so it always succeeds...
   pthread_create(&outputWriterThread,&attr,&outputHandler<Writer>::startOutputWriterThread,m_pOutputHandler);

}

//=============================================================================
template <class Writer>
logMngr<Writer>::~logMngr()
{
   delete (m_pOutputHandler);
   m_pOutputHandler = 0;
}

//=============================================================================
template <class Writer>
int logMngr<Writer>::write (const char *const  i_pMsgText, const char *const  i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,bool i_writeStack)
{
   unsigned int curIndex = 0;
   unsigned int curLifeID = 0;
   msgTokenMngr::msg_token_t entryIdentifier = 0;

   m_msgTokenMngr.getNextIndex(curIndex,curLifeID,entryIdentifier);

   if (i_severity >= m_flushSeverity)
   {
      bool shouldReleaseWriter = this->m_flushTokenHolder.isEmpty();
      this->m_flushTokenHolder.addToken(entryIdentifier);

      //It is ugly that this is here, and not done automatically bu the flushTokenHolder,
      //However, in order for the flushTokenHolder not to be aware of the outputHandler we 
      //shold either do it from here, or, install observer in the flushTokenHolder.
      //For now, I'll live this ugliness, for simplicity...
      if (shouldReleaseWriter)
      {
         m_pOutputHandler->release();
      }
   }

   PRINT_DEBUG(1, entryIdentifier
         << "|Severity: " <<i_severity 
         << " Time:  "     <<i_time 
         << " ThreadID: "   <<i_tid 
         << " FuncName: "   << i_pFuncName 
         << " Message: "   <<i_pMsgText);
   typename logMsgEntity<Writer>::resultStatus res = logMsgEntity<Writer>::RS_Unset;
   do {
      res = m_msgs[curIndex].set(i_pMsgText,i_pFuncName,i_time,i_tid,i_severity,curLifeID,i_writeStack);
      //TODO need to handle/yield in case od retry,....
   } while(res != logMsgEntity<Writer>::RS_Success );
   return 0;
}

//============================================================================
template <class Writer>
void logMngr<Writer>::shutDown()
{
   PRINT_DEBUG (2, "******Calling shutDown *******");

   m_flushTokenHolder.addToken(SHUTDOWN_ENTRY);

   //Wait for the writer to finish.
   m_pOutputHandler->waitForOutputToComplete();
   loggerStatistics::instance()->print();

}

