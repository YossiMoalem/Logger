#ifndef MSG_DATA_H
#define MSG_DATA_H

#include "stackwalker.h"

#define MAX_MSG_TEXT_SIZE  200
#define MAX_FUNC_NAME_SIZE 100
#define STACK_SIZE 5

struct msgData
{
   public:
   msgData ()
   {
      m_msgText[MAX_MSG_TEXT_SIZE] = 0;
      m_funcName[MAX_FUNC_NAME_SIZE] = 0;
   }

   public:
   char 		        m_msgText [ MAX_MSG_TEXT_SIZE+ 1 ];
   char 		        m_funcName [ MAX_FUNC_NAME_SIZE + 1 ];
   Stackwalker::stackFrameAddr  m_stack[STACK_SIZE];
   time_t 		        m_time;
   pid_t		        m_tid;
   int 			        m_severity;
};

#endif //MSG_DATA_H
