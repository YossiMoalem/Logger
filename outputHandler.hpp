#include "outputHandler.h"
#include "msgTokenMngr.h"
#include "loggerStatistics.h"
#include "logMngr.h"
#include "utils.h"
#include <sched.h>


template <class Writer>
void* outputHandler<Writer>::startOutputWriterThread(void *i_outputHandler)
{
   (static_cast <outputHandler<Writer>*> (i_outputHandler)) -> startMainLoop();
   return 0;
}

template <class Writer>
void outputHandler<Writer>::startMainLoop ()
{
   sem_init (&m_shutDownSem, 0, 1);
   sem_wait(&m_shutDownSem);

   sem_init (&m_isEmptySem, 0, 1);

   //True untill we get shutdowm msg.
   bool shouldContinue  = true;

   while (true == shouldContinue)
   {
      sem_wait(&m_isEmptySem);
      loggerStatistics::instance()->inc_counter(loggerStatistics::outputWriter_writeMsgAttempts);
      while (!m_flushTokenHolder.isEmpty())
      {
         msgTokenMngr::msg_token_t  entryIdentifier = m_flushTokenHolder.getToken();

         if (IS_SHUTDOWN_IDENT(entryIdentifier))
         {
            PRINT_DEBUG (2, "Popped Shutdown Msg");
            shouldContinue = false;
            break;
         }

         unsigned int startIndex = GET_CUR_INDEX(entryIdentifier);
         unsigned int startLifeID = GET_CUR_LIFE_ID(entryIdentifier);
         unsigned int expectedLifeID = startLifeID;
         int curIndex =  startIndex - NUM_OF_RECORDS_TO_FLUSH + 1;
         if (curIndex < 0)
         {
            //this is the first round, start from the begining of the log
            if (startLifeID == 1)
            {
               curIndex = 0;
            }
            else
            {
               curIndex += NUM_OF_LOG_MSGS;
               --expectedLifeID;
            }
         }

         PRINT_DEBUG (2, "Popped identifier " <<entryIdentifier
               <<"(Index : " <<startIndex 
               <<" LifeID : " <<startLifeID <<")"
               <<" Start Index: " << curIndex 
               <<" STart Life ID: " << expectedLifeID);
         m_pWriter->startBlock();


         bool lastPrinted = false;

         while ( false == lastPrinted )
         {
            printSingleMsg(curIndex, expectedLifeID);
            lastPrinted = ((unsigned int) curIndex == startIndex);

            ++curIndex;
            if (curIndex >= NUM_OF_LOG_MSGS)
            {   
               curIndex -= NUM_OF_LOG_MSGS;
               expectedLifeID++;
            }   

         }
      }//end while queue not empty
   }// end while should continue
   sem_post(&m_shutDownSem);
}

template <class Writer>
void outputHandler<Writer>::printSingleMsg (unsigned int index, unsigned int expectedLifeID)
{
   typename logMsgEntity<Writer>::resultStatus retval = logMsgEntity<Writer>::RS_Success;
   do {
      retval = m_msgs[index].write(m_pWriter, expectedLifeID);
      if (retval == logMsgEntity<Writer>::RS_MsgNotYetWriten)
      {
         sched_yield();
#if DEBUG >= 7 || defined STATISTICS
         static int cpuYieldCounter = 0;
         ++ cpuYieldCounter; 
         cpuYieldCounter %= 1024;
         if (cpuYieldCounter == 0)
         {
            loggerStatistics::instance()->inc_counter(loggerStatistics::outputWriter_CpuYield);
            PRINT_DEBUG (7, "CPU yield 1024 times for expected LID " << expectedLifeID 
                  <<" Index = " << index 
                  <<" Expected Token:" << CREATE_ENTRY_IDENT(expectedLifeID, index))
         }
#endif   //DEBUG >= 7 || defined STATISTICS                  
      } //logMsgEntity::RS_MsgNotYetWriten

      if (retval == logMsgEntity<Writer>::RS_MsgOverwritten)
      {
         loggerStatistics::instance()->inc_counter(loggerStatistics::outputWriter_overwrittenMsgs);
         m_pWriter->writeError("the entry was overwritten before the flush");
         PRINT_DEBUG(4, "Message index " <<index<<" was overwritten before flash."
               <<"Expected LID = " <<expectedLifeID );
      }

   } while(retval == logMsgEntity<Writer>::RS_MsgNotYetWriten);
}

template <class Writer>
void outputHandler<Writer>::waitForOutputToComplete ()
{
   //Because the LogMngr calls startFlushing() from write()
   //if the queue is empty while inserting the shutdown msg, 
   //the semaphore willnot be released. Therefoe we call it from here as well...
   startFlushing();
   //Wait here untill the semaphore is unlocked. 
   //Once it is unlocked, the main loop has finished...
   sem_wait(&m_shutDownSem);
   sem_post(&m_shutDownSem);
}
