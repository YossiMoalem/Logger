# include "outputHandler.h"
#include "entityIdentifierType.h"
#include "logMngr.h"
#include <sched.h>

outputHandler::outputHandler(logMngr & i_logMngr):m_logMngr(i_logMngr)
{
}


int outputHandler::doSomething()
{
   bool shouldContinue = true;
   while (true == shouldContinue)
   {
      // TODO change the busy loop on the empty queue condition
      while (!m_logMngr.m_queueFlushStartIndex.empty())
      {
         entityIdentifierType::entity_identifier_t  entryIdentifier = m_logMngr.m_queueFlushStartIndex.front();
         m_logMngr.m_queueFlushStartIndex.pop();
         
         if (IS_SHUTDOWN_IDENT(entryIdentifier))
         {
            shouldContinue = false;
            break;

         }

         unsigned int startIndex = GET_CUR_INDEX(entryIdentifier);
         unsigned int startLifeID = GET_CUR_LIFE_ID(entryIdentifier);
         unsigned int expectedLifeID = startLifeID;

#if DEBUG >= 2
         PRINT_DEBUG ("Poped identifier " <<entryIdentifier
               <<"(Index : " <<startIndex 
               <<"LifeID : " <<startLifeID );
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
         m_logMngr.startBlock();
         bool shouldContinue = true;
         bool lastPrinted = false;

         while (true == shouldContinue)
         {
            int cpuYieldCounter = 0;
            while (expectedLifeID >  m_logMngr.m_msgs[curIndex].getLifeID())
            {
#if DEBUG >= 7
               const int mask = ~(1024);
               ++ cpuYieldCounter; 
               cpuYieldCounter &= mask;
               if (cpuYieldCounter == 0)
               {
                  PRINT_DEBUG ("CPU yield 1024 times for expected LID " << expectedLifeID <<" Got LID: " <<m_logMngr.m_msgs[curIndex].getLifeID() <<" Index = " << curIndex <<" Expected LID:" << CREATE_ENTRY_IDENT(expectedLifeID, curIndex))
               }
#endif
               sched_yield();
            }

            if (m_logMngr.m_msgs[curIndex].getLifeID() > expectedLifeID)
            {
               m_logMngr.writeError("the entry was overwritten during the flush");
            }
            else
            {   
               m_logMngr.m_msgs[curIndex].write(m_logMngr.m_pLogMsgFormatterWriter,expectedLifeID);
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
   pthread_mutex_unlock(&(m_logMngr.m_shutDownMutex));
   return 0;
}
