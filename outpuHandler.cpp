#include "outputHandler.h"
#include "msgTokenMngr.h"
#include "loggerStatistics.h"
#include "logMngr.h"
#include "utils.h"
#include <sched.h>

outputHandler::outputHandler()
{
}


void *  outputHandler::startOutputWriterThread(void *i_logMngr)
{
   bool shouldContinue = true;
  logMngr*  pLogMngr = static_cast <logMngr*> (i_logMngr);
   while (true == shouldContinue)
   {
      // TODO change the busy loop on the empty queue condition
      while (!pLogMngr->m_queueFlushStartIndex.empty())
      {
         msgTokenMngr::msg_token_t  entryIdentifier = pLogMngr->m_queueFlushStartIndex.front();
         pLogMngr->m_queueFlushStartIndex.pop();
         
         if (IS_SHUTDOWN_IDENT(entryIdentifier))
         {
            shouldContinue = false;
            break;
         }

         unsigned int startIndex = GET_CUR_INDEX(entryIdentifier);
         unsigned int startLifeID = GET_CUR_LIFE_ID(entryIdentifier);
         unsigned int expectedLifeID = startLifeID;

#if DEBUG >= 2
         PRINT_DEBUG ("Popped identifier " <<entryIdentifier
               <<"(Index : " <<startIndex 
               <<"LifeID : " <<startLifeID <<")");
#endif

         int curIndex =  startIndex - NUM_OF_RECORDS_TO_FLUSH + 1;
         if (curIndex < 0)
         {
            if (startLifeID == 1)
            {
               //The first message will be inserted in index 1
               curIndex = 1;
            }
            else
            {
               curIndex+=NUM_OF_LOG_MSGS;
               --expectedLifeID;
            }
         }
         pLogMngr->startBlock();
         bool shouldContinue = true;
         bool lastPrinted = false;

         while (true == shouldContinue)
         {
            int cpuYieldCounter = 0;
            //TODO: RACE!!!!
            while (expectedLifeID >  pLogMngr->m_msgs[curIndex].getLifeID())
            {
#if DEBUG >= 7 || defined STATISTICS
               ++ cpuYieldCounter; 
               cpuYieldCounter %= 1024;
               if (cpuYieldCounter == 0)
               {
                  loggerStatistics::instance()->inc_counter(loggerStatistics::outputWritter_CpuYield);
	#if DEBUG >= 7
                  PRINT_DEBUG ("CPU yield 1024 times for expected LID " << expectedLifeID 
                                 <<" Got LID: " <<pLogMngr->m_msgs[curIndex].getLifeID() 
                                 <<" Index = " << curIndex 
                                 <<" Expected Token:" << CREATE_ENTRY_IDENT(expectedLifeID, curIndex))
	#endif   // DEBUG >= 7          
			   }
#endif	//DEBUG >= 7 || defined STATISTICS
               sched_yield();
            }

            if (pLogMngr->m_msgs[curIndex].getLifeID() > expectedLifeID)
            {
               loggerStatistics::instance()->inc_counter(loggerStatistics::outputWritter_overwrittenMsgs);
               pLogMngr->writeError("the entry was overwritten during the flush");
#if DEBUG >= 4
               PRINT_DEBUG("Message index " <<curIndex<<" was overwritten during flash."
                     <<"Expected LID = " <<expectedLifeID 
                     <<" got LID : " << pLogMngr->m_msgs[curIndex].getLifeID());
#endif
            }
            else
            {   
               pLogMngr->m_msgs[curIndex].write(pLogMngr->m_pLogMsgFormatterWriter,expectedLifeID);
            }
            
            ++curIndex;
            if (curIndex >= NUM_OF_LOG_MSGS)
            {   
               curIndex -= NUM_OF_LOG_MSGS;
               expectedLifeID++;
            }   
            
            if (lastPrinted == true)
            {
               shouldContinue = false;
            }
            lastPrinted = ((unsigned int) curIndex == startIndex);
         }
      }//end while queue not empty
   }// end while should continue
   pthread_mutex_unlock(&(pLogMngr->m_shutDownMutex));
   return 0;
}
