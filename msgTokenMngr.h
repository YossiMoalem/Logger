#ifndef MSG_TOKEN_MNGR_H
#define MSG_TOKEN_MNGR_H

#include <boost/static_assert.hpp>
#include <assert.h>         /* assert */
#include "loggerStatistics.h"


#define NUM_OF_LOG_MSGS  1000
//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);
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
        class msg_token_t
        {
         public:
           typedef long long valueType ;
           msg_token_t( ) = default;
           msg_token_t( long long value ):
               m_value( value )
            {}
           operator long long () const
           {
               return m_value;
           }

           long long get()
           {
               return m_value;
           }

           msg_token_t & operator -= ( int amount )
           {
               int newIndex = GET_CUR_INDEX( m_value ) - amount;
               unsigned int newLID = GET_CUR_LIFE_ID ( m_value );
               if ( newIndex < 0 )
               {
                   //this is the first round, start from the begining of the buffer
                   if ( newLID == 1)
                   {
                       newIndex = 0;
                   } else {
                       newIndex += NUM_OF_LOG_MSGS;
                       --newLID;
                   }
                   m_value = CREATE_ENTRY_IDENT (newLID,newIndex);
                   return * this;
               }
           }

           
           msg_token_t operator++()
           {
               int newIndex = GET_CUR_INDEX( m_value ) + 1;
               unsigned int newLID = GET_CUR_LIFE_ID ( m_value );
               if (newIndex >= NUM_OF_LOG_MSGS)
               {   
                   newIndex -= NUM_OF_LOG_MSGS;
                   ++newLID;
               }   
               m_value = CREATE_ENTRY_IDENT (newLID,newIndex);
               return *this;
           }

         private:
           long long m_value;
        };

      msgTokenMngr(unsigned int i_numOfMsgs): m_numOfMsgs(i_numOfMsgs),
      m_curEntryIndent(CREATE_ENTRY_IDENT(1,0))
   {}

      void getNextIndex ( msg_token_t &o_entryIdentifier ); 

   private:
   /* Non-Copyable */ 
   msgTokenMngr (const msgTokenMngr&);
   msgTokenMngr& operator= (const msgTokenMngr&);

   private:
      unsigned int                  m_numOfMsgs;
      msg_token_t::valueType       m_curEntryIndent;
};

BOOST_STATIC_ASSERT ((sizeof(msgTokenMngr::msg_token_t ) >= 8));


inline void msgTokenMngr::getNextIndex ( msg_token_t &o_entryIdentifier ) 
{ 
   msg_token_t currentToken  = 0;
   msg_token_t newToken ;

   do 
   {
      loggerStatistics::instance()->inc_counter(loggerStatistics::msgTokenMngr_failedAttempts);
      currentToken = m_curEntryIndent ;
      o_entryIdentifier = currentToken;

      unsigned int next_index = GET_CUR_INDEX( currentToken ) + 1;
      unsigned int next_LID = GET_CUR_LIFE_ID (currentToken);
      
      if (m_numOfMsgs == next_index)
      {   
         next_index = 0;
         next_LID++;
      }
      assert (m_numOfMsgs>next_index);
      newToken = CREATE_ENTRY_IDENT (next_LID,next_index);
   } while (! __sync_bool_compare_and_swap (&m_curEntryIndent, currentToken.get(), newToken.get()));
   
   loggerStatistics::instance()->dec_counter(loggerStatistics::msgTokenMngr_failedAttempts);
}

#endif
