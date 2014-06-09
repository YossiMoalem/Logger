#include "outputHandler.h"
#include "utils.h"

#include <pthread.h> 


//============================================================================
template <class Writer>
logMngr<Writer>::logMngr(int i_flushSeverity) : 
   m_pWriter(new Writer),
   m_flushSeverity(i_flushSeverity),
   m_msgTokenMngr(NUM_OF_LOG_MSGS)
{
   loggerStatistics::instance(); //Just in order to create the statistics mngr....

   sem_init (&m_shutDownSem, 0, 1);
   sem_wait(&m_shutDownSem);

   pthread_t                   outputWriterThread;
   pthread_attr_t attr;
   
   int s = pthread_attr_init(&attr);
   if (s != 0)
   {
      m_pWriter->write_error ("Error while trying to set writer thread attribute. Logger will be disabled!");
   }
   else
   {
      s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      if (s != 0)
      {
         m_pWriter->write_error("Error while trying to set thread as detachable. Logger will be disabled");
      } else {
         pthread_create(&outputWriterThread,&attr,&outputHandler<Writer>::startOutputWriterThread,this);
      }
   }

}

//=============================================================================
template <class Writer>
logMngr<Writer>::~logMngr()
{
   delete (m_pWriter); 
   m_pWriter = 0;
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
      this->m_flushTokenHolder.addToken(entryIdentifier);
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
   sem_wait(&m_shutDownSem);

   loggerStatistics::instance()->print();

}

