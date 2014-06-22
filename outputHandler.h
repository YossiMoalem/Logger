#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

/******************************************************************************\
 *
 * This class is responsible for poping flash token from the flushTokenHolder
 * and flushing the required number of messages from this index backwords.
 *
 * It loops untill it gets shutdown token, and everytime there is 
 * a flush token available - it ftart the flushing.
 *
 * It runs on a dedicated thread, created by the logMngr
 ******************************************************************************/
template <class Writer>
class outputHandler
{
   public:
      outputHandler( logMsgEntity<Writer> (&i_msgs)[NUM_OF_LOG_MSGS],
                     FlushTokensHolder&   i_flushTokenHolder):
                  m_msgs(i_msgs),
                  m_flushTokenHolder(i_flushTokenHolder),
                  m_pWriter(new Writer)
      {}

      /* Entry point for the created thread */
      static void * startOutputWriterThread (void * i_logMngr);

      /************************************************************************\
       * It is possible that we still  have messages to flush, even when the 
       * application terminates.
       * This function should be called prior to terminating the app.
       * It will block untill all messages are flused.
       ************************************************************************/
      void waitForOutputToComplete ();

      /************************************************************************\
       * Indicate that there are messages ready for flushing
       * This will start the outputWritter thread flush.
       * As this is dedicated thread, this is non blocking, non sync
       ************************************************************************/
      void startFlushing() { sem_post(&m_isEmptySem);}

      ~outputHandler()
      {
         delete (m_pWriter); 
         m_pWriter = 0;
      }

   /*****************************************************************************\
    * getWriter
    * Returns the writter, so application can call writter specific methods (e.g. init())
    *****************************************************************************/
   Writer* getWriter ()
   {
      return m_pWriter;
   }
   private:
      void startMainLoop ();
      void printSingleMsg (unsigned int index, unsigned int expectedLifeID);

      /* Non-Copyable */ 
      outputHandler (const outputHandler&);
      outputHandler& operator= (const outputHandler&);

   private:
      logMsgEntity<Writer>  (&m_msgs)[NUM_OF_LOG_MSGS] ;
      FlushTokensHolder&    m_flushTokenHolder;
      Writer*               m_pWriter;
      sem_t                 m_shutDownSem;
      sem_t                 m_isEmptySem;
};

#include "outputHandler.hpp"

#endif 

