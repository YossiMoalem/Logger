#ifndef LOG_MSG_ENTITY
#define LOG_MSG_ENTITY

#include <sys/types.h>  // for pid_t
#include <time.h> //for time_t
#include <pthread.h>//for the mutex

#include "msgData.h"
#include "logMsgFormatterWriter.h"



/******************************************************************************\
 * logMsgEntity:
 * This is the basic "building block" of the logger.
 * It holds the log msg data, and some metadata, for a single message. 
 * The main two methods here are set(), to record a new message, and write, 
 * to flush a message().
 *
 ******************************************************************************/

template <class Writer>
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
   resultStatus  write (Writer* i_logMsgFormatterWriter,
                        unsigned int i_expectedLifeID);

   private:
   /* This must NOT be public. Usage of this function requires LOCKing the cell!! 
    * It is only here to enforce constness */
   const msgData& getMsgData () { return m_msgData;}

   /* Non Copyable */
   logMsgEntity (const logMsgEntity&);
   logMsgEntity& operator=(const logMsgEntity&);

   private :
   msgData              m_msgData;
   msgState 		m_state;
   unsigned int         m_lifeID;
   

#ifndef NOLOCK
   pthread_mutex_t     m_lock;
#endif    
};
#include "logMsgEntity.hpp"
#endif
