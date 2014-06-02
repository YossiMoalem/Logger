#ifndef MSG_TOKEN_MNGR_H
#define MSG_TOKEN_MNGR_H

#include <boost/static_assert.hpp>
#include <assert.h>         /* assert */
#include "loggerStatistics.h"


#define WORD_SIZE 32 
#define WORD_MAX_VAL 0xFFFFFFFF
#define GET_CUR_INDEX(ENTRY_IDENT)  (((msgTokenMngr::msg_token_t) (ENTRY_IDENT)) & WORD_MAX_VAL)
#define GET_CUR_LIFE_ID(ENTRY_IDENT) (((msgTokenMngr::msg_token_t) (ENTRY_IDENT)) >> WORD_SIZE)
#define CREATE_ENTRY_IDENT(LIFE_ID,INDEX) ((((msgTokenMngr::msg_token_t) (LIFE_ID)) << WORD_SIZE) | (INDEX & WORD_MAX_VAL))
#define SHUTDOWN_ENTRY (CREATE_ENTRY_IDENT(0,0))
#define IS_SHUTDOWN_IDENT(IDENT) ((msgTokenMngr::msg_token_t) (IDENT)  == 0) 

/******************************************************************************
 * msgTokenMngr:
 * Each message aquire a token, using getNextIndex().
 * This token is used to determine the cell to which the message will be written to, 
 * and it also provides "Life ID" (LID), that is used to determine if the message 
 * currently in the cell, is the one we are interested in, was it overwritten, or, 
 * was it not written yet.
 * In addition, it creates a "shutdown" token.
 * When this token is sent to the output-writer, it flushes all messages, and shuts down.
 *
 * TODO: Currently, overflow of LID is not handled. 
 ******************************************************************************/


class msgTokenMngr
{
 		public:
        typedef unsigned long long msg_token_t ;       
        
        msgTokenMngr(unsigned int i_numOfMsgs): m_numOfMsgs(i_numOfMsgs),
                                                m_curEntryIndent(CREATE_ENTRY_IDENT(1,0))
   {}
        
        void getNextIndex (unsigned int &o_index,unsigned int& o_lifeID,msg_token_t &o_entryIdentifier); 

      private:
      unsigned int m_numOfMsgs;
       msg_token_t         m_curEntryIndent;
};

BOOST_STATIC_ASSERT ((sizeof(msgTokenMngr::msg_token_t ) >= 8));


inline void msgTokenMngr::getNextIndex (unsigned int &o_index,unsigned int& o_lifeID,msg_token_t &o_entryIdentifier) 
{ 
   msg_token_t oldVal  = 0;
   do 
   {
      loggerStatistics::instance()->inc_counter(loggerStatistics::msgTokenMngr_failedAttempts);
      oldVal = m_curEntryIndent ;
      o_index  = GET_CUR_INDEX (oldVal);
      o_lifeID = GET_CUR_LIFE_ID (oldVal);
      o_index++ ;
      
      if (m_numOfMsgs == o_index)
      {   
         o_index = 0;
         o_lifeID++;
      }
      assert (m_numOfMsgs>o_index);
      o_entryIdentifier = CREATE_ENTRY_IDENT (o_lifeID,o_index);
   } while (! __sync_bool_compare_and_swap (&m_curEntryIndent, oldVal, o_entryIdentifier));
   
   loggerStatistics::instance()->dec_counter(loggerStatistics::msgTokenMngr_failedAttempts);
}




#endif
