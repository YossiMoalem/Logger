#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"
#include "msgTokenMngr.h"
#include "flushTokensHolder.h"


#include <semaphore.h>

#define NUM_OF_RECORDS_TO_FLUSH 20 
#define NUM_OF_LOG_MSGS  (NUM_OF_RECORDS_TO_FLUSH*10)

//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);
/*****************************************************************************\
 * logMngr:
 * This is the main interface/entry point for the logger.
 * Template Param Writer is the class that will do the actual printing.
 * Please look at logMsgFormatterWriter.h for the intf. description
 *
 * It provides write(), in order to log messages,
 * and, shutDown(), to gracefully shut down the logger
 *
 * Note:
 * Application should use the macros provided in smartLogger.
 *
 * TODO: move all the characteristics to a new, traites class.
 * TODO: move the filter fo be policy
 *****************************************************************************/

template <class Writer>
class outputHandler;

template <class Writer>
class logMngr
{
   //The outputHandler should access the logMessages
   friend class outputHandler<Writer>;

   public:

   /*****************************************************************************\
    * Create a new logMngr inst.
    * i_flushSeverity: messages with severity higher or equal to this severity, 
    *       will initiate flush.
    *****************************************************************************/
   logMngr (int i_flushSeverity);
   ~logMngr ()

   /*****************************************************************************\
    * Log a new message.
    * If severity is equal to, or higher than the flush severity, 
    * this will initiate flush.
    * Otherwise, this will only record the message.
    *
    * If i_writeStack is true, stack will be added to this message.
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
    * getWritter
    * Returns the writter, so application can call writter specific methods (e.g. init())
    *****************************************************************************/
   Writer* getWritter ()
   {
      return m_pWriter;
   }

   private:     

   /*****************************************************************************\
    * write a block seperator 
    * (block is a set of flushed messages)
    *****************************************************************************/
   void startBlock();


   /*****************************************************************************\
    * getFlusTokenMngr:
    * Returns the FlushTokensHolder, so the writer can pop tokens to flush.
    *****************************************************************************/
   FlushTokensHolder& getFlusTokenMngr ()
   {
      return m_flushTokenHolder;
   }
   /* Non-Copyable */ 
   logMngr(const logMngr&);
   logMngr operator= (const logMngr&);


   private:

   logMsgEntity<Writer>         m_msgs[NUM_OF_LOG_MSGS] ;
   Writer*                      m_pWriter;
   int 		                m_flushSeverity;
   msgTokenMngr                 m_msgTokenMngr;
   FlushTokensHolder            m_flushTokenHolder;
   sem_t                        m_shutDownSem;
};

#include "logMngr.hpp"
#endif
