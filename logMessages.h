#ifndef LOG_MESSAGES_H
#define LOG_MESSAGES_H

#include "logMsgEntity.h"
#include "msgTokenMngr.h"



template <class Writer>
class logMessages
{
   public:
   logMessages () {}

   typename logMsgEntity<Writer>::resultStatus set( msgTokenMngr::msg_token_t token,
           const char *const  i_pMsgText, 
           const char *const  i_pFuncName, 
           time_t i_time, 
           pid_t i_tid, 
           int i_severity,
           bool i_writeStack )
   {
       unsigned int index  = GET_CUR_INDEX ( token );
       unsigned int lifeID = GET_CUR_LIFE_ID ( token );
       typename logMsgEntity<Writer>::resultStatus res = logMsgEntity<Writer>::RS_Unset;
       res = m_msgs[ index ].set(i_pMsgText,i_pFuncName,i_time,i_tid,i_severity,lifeID,i_writeStack);
       return res;
   }

   typename logMsgEntity<Writer>::resultStatus write( Writer * writer,
           msgTokenMngr::msg_token_t token )
   {
       unsigned int index  = GET_CUR_INDEX ( token );
       unsigned int lifeID = GET_CUR_LIFE_ID ( token );
       typename logMsgEntity<Writer>::resultStatus res = logMsgEntity<Writer>::RS_Unset;
       res = m_msgs[ index ].write( writer, lifeID );
       return res;
   }

   private:
   /* Non-Copyable */ 
   logMessages (const logMessages&);
   logMessages operator = (const logMessages&);

   logMsgEntity<Writer>& operator [] (unsigned int index)
   {
      assert(index < NUM_OF_LOG_MSGS);
      return m_msgs[index];
   }

   private:
   logMsgEntity<Writer>         m_msgs[NUM_OF_LOG_MSGS] ;
};

#endif
