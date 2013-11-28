# include "outputHandler.h"
#include "logMngr.h"
#include <iostream> //TODO remove this

outputHandler::outputHandler(logMngr & i_logMngr):m_logMngr(i_logMngr)
{
}


int outputHandler::doSomething()
{
   while (1)
   {
      while (!m_logMngr.m_queueFlushStartIndex.empty())
      {
         int startIndex = m_logMngr.m_queueFlushStartIndex.front();
         unsigned int startLifeID = m_logMngr.m_msgs[startIndex].getLifeID();
         unsigned int expectedLifeID = startLifeID;
         m_logMngr.m_queueFlushStartIndex.pop();

         int curIndex =  startIndex - NUM_OF_RECORDS_TO_FLUSH;
         if (curIndex < 0)
         {
            if (startLifeID==1)
            {
               curIndex =0;
            }
            else
            {
               curIndex+=NUM_OF_LOG_MSGS;
               expectedLifeID--;
            }
         }
         m_logMngr.startBlock();
         while (startIndex != curIndex)
         {
            //TODO add condition in order to remove the busy loop

            while (expectedLifeID >  m_logMngr.m_msgs[curIndex].getLifeID());

            if (m_logMngr.m_msgs[curIndex].getLifeID() > expectedLifeID)
            {
               m_logMngr.writeError("the was overwritten during the flush");
            }
            else
            {   
               m_logMngr.m_msgs[curIndex].write(m_logMngr.m_pLogMsgFormatterWriter);

            }
            ++curIndex;
            if (curIndex >= NUM_OF_LOG_MSGS)
            {   
               curIndex -= NUM_OF_LOG_MSGS;
               expectedLifeID++;
            }   
         }
         m_logMngr.m_msgs[curIndex].write(m_logMngr.m_pLogMsgFormatterWriter);
      }
   }
}
