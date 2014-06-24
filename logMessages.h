#ifndef LOG_MESSAGES_H
#define LOG_MESSAGES_H

#include "logMsgEntity.h"

#define NUM_OF_LOG_MSGS  1000

//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);

template <class Writer>
class logMessages
{
   public:
   logMessages () {}
   logMsgEntity<Writer>& operator [] (unsigned int index)
   {
      assert(index < NUM_OF_LOG_MSGS);
      return m_msgs[index];
   }


   private:
   /* Non-Copyable */ 
   logMessages (const logMessages&);
   logMessages operator = (const logMessages&);

   private:
   logMsgEntity<Writer>         m_msgs[NUM_OF_LOG_MSGS] ;

};

#endif
