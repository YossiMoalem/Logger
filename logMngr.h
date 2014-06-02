#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"
#include "msgTokenMngr.h"
#include "flushTokens.h"

#include <pthread.h> 
#include <semaphore.h>

#define NUM_OF_RECORDS_TO_FLUSH 20 
#define NUM_OF_LOG_MSGS  (NUM_OF_RECORDS_TO_FLUSH*10)

/*****************************************************************************\
 * logMngr:
 * This is the main interface of the logger, and the only one that the application 
 * should use.
 *
 * Application should create instance of this class,
 * Call write(), in order to log messages,
 * and, finally, call shutDown, when the application terminated.
 *
 * TODO: move the formatterWriter to be template argument
 * TODO: move all the characteristics to a new, traites cklass.
 *****************************************************************************/

class logMngr
{
   //The outputHandler should access the logMessages
   friend class outputHandler;

   public:

   /*****************************************************************************\
    * Create a new logMngr inst.
    * i_flushSeverity: messages with severity higher or equal to this severity, 
    *       will initiate flush.
    * i_pLogMsgFormatterWriter: the class that will do the actual writing.
    *****************************************************************************/
   logMngr (int i_flushSeverity,  
         logMsgFormatterWriter* i_pLogMsgFormatterWriter );

   /*****************************************************************************\
    * Log a new message.
    * If severity is equal to, or higher than the flush severity, 
    * this will initiate flush.
    * Otherwise, this will only record the message.
    *
    * If i_writeStack is true, stack will be added to this message.
    *
    * TODO: Can remove TID, and get it automaticvally??
    *****************************************************************************/
   int write (const char *const     i_pMsgText, 
         const char*const  i_pfuncName, 
         time_t            i_time, 
         pid_t             i_tid, 
         int               i_severity,
         bool              i_writeStack);

   /*****************************************************************************\
    * "Shut down" the logger.
    * before the application terminates, make sure there are no more messages that
    * needs to be written.
    * Note:
    * Calling write() after shutDown() will cause unspecified behaviour, and should be avoided.
    *****************************************************************************/
   void shutDown ();


   /*****************************************************************************\
    * getFlusTokenMngr:
    * Returns the FlushTokens Manager, so the writer can pop tokens to flush.
    *****************************************************************************/
   FlushTokens& getFlusTokenMngr ()
   {
      return m_flushTokenMngr;
   }

   private:     

   /*****************************************************************************\
    * write a block seperator 
    * (block is a set of flushed messages)
    *****************************************************************************/
   void startBlock();

   /*****************************************************************************\
    * In case of logger error - log it.
    *****************************************************************************/
   void writeError (const char* i_pErrorMessage);

   /* Non-Copyable */ 
   logMngr(const logMngr&);
   logMngr operator= (const logMngr&);

   private:

   logMsgEntity                 m_msgs[NUM_OF_LOG_MSGS] ;
   int 		                m_flushSeverity;
   logMsgFormatterWriter*       m_pLogMsgFormatterWriter;
   msgTokenMngr                 m_msgTokenMngr;
   FlushTokens                  m_flushTokenMngr;

   sem_t                        m_shutDownSem;
};

 #endif
