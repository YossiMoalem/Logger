#ifndef LOG_MSG_ENTITY
#define LOG_MSG_ENTITY

#include <sys/types.h>  // for pid_t
#include <time.h> //for time_t
#include <string.h>//for strncpy
#include <stdio.h> //for sprintf
#include <assert.h> //for assert...
#include <pthread.h>//for the mutex

#include "logMsgFormatterWriter.h"
#include "stackwalker.h"


#define MAX_MSG_TEXT_SIZE  200
#define MAX_FUNC_NAME_SIZE 100
#define STACK_SIZE 5

/******************************************************************************\
 * logMsgEntity:
 * This is the basic "building block" of the logger.
 * It holds all the log msg data, and some metadata, for a single message. 
 * The main two methods here are set(), to record a new message, and write, 
 * to flush a message().
 *
 ******************************************************************************/

class logMsgEntity
{
   /******************************************************************************\
    * The Current Msg state
    ******************************************************************************/
   enum msgState
   {
      MS_Unused 	    = 0, //Has not been used yet
      MS_InProgress 	    = 1, //Massage is being set now
      MS_Ready	            = 2, //Message has been set, and can be flushed
      MS_BeingFlushed       = 3, //Message is being flushed now
      MS_Flushed	    = 4  //Message has been flused, and can be overwritten. 
   };
   public:

   enum resultStatus
   {
      RS_Unset 		   = 0, //Status was not set...
      RS_Success           = 1, //Opperation completed successfully
      RS_MsgOverWritten    = 2, //Msg has been overwritten already
      RS_MsgNotYetWriten   = 3, //the required message was not written yet
      RS_MsgBusy           = 4, //The msg is in process not (either, being set or being flushed)
      RS_InternalError     = 5, //Internal error...
      RS_Last
   };

   logMsgEntity();

   /******************************************************************************\
    * Set a new message.
    * life_id should be extracted from the msg token.
    * If i_writeStack == true, stackWalker will be used in order to collect stack trace.
    ******************************************************************************/
   resultStatus set (const char*    i_pNewMsg, 
                     const char*    i_pFuncName, 
                     time_t         i_time, 
                     pid_t          i_tid, 
                     int            i_severity,
                     unsigned int   i_lifeID,
                     bool           i_writeStack);

   /******************************************************************************\
    * Flush the current log message.
    ******************************************************************************/
   resultStatus  write (logMsgFormatterWriter* i_logMsgFormatterWriter,
                        unsigned int i_expectedLifeID);

   private:
   /* Non Copyable */
   logMsgEntity (const logMsgEntity&);
   logMsgEntity& operator=(const logMsgEntity&);

   private :

   msgState 			m_state;
   char 				m_msgText [ MAX_MSG_TEXT_SIZE+ 1 ];
   char 				m_funcName [ MAX_FUNC_NAME_SIZE + 1 ];
   time_t 				m_time;
   pid_t				m_tid;
   int 				m_severity;
   unsigned int        m_lifeID;
   Stackwalker::stackFrameAddr m_stack[STACK_SIZE];

#ifndef NOLOCK
   pthread_mutex_t     m_lock;
#endif    
};

#endif
