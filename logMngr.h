#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMessages.h"
#include "msgTokenMngr.h"
#include "flushTokensHolder.h"


#include <semaphore.h>

#define NUM_OF_RECORDS_TO_FLUSH 20
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

   public:

   /*****************************************************************************\
    * Create a new logMngr inst.  i_flushSeverity: messages with severity
    * higher or equal to this severity, will initiate flush.
    *****************************************************************************/
    logMngr (int i_flushSeverity); 
    ~logMngr ();

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
    * getWriter
    * Returns the writter, so application can call writter specific methods (e.g. init())
    *****************************************************************************/
   Writer* getWriter ()
   {
      return m_pOutputHandler->getWriter();
   }

   private:     

   /* Non-Copyable */ 
   logMngr(const logMngr&);
   logMngr operator= (const logMngr&);


   private:

   logMessages <Writer>         m_msgs;
   int 		                m_flushSeverity;
   msgTokenMngr                 m_msgTokenMngr;
   FlushTokensHolder            m_flushTokenHolder;
   outputHandler<Writer>*       m_pOutputHandler;
};

#include "logMngr.hpp"
#endif
