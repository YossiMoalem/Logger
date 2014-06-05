#ifndef LOG_MSG_FORMATTER_WRITER
#define LOG_MSG_FORMATTER_WRITER

#include "msgData.h"

#include <stdio.h>

class logMsgFormatterWriter
{
   public:
      virtual void write (const msgData& i_msgData) = 0;
      virtual void writeWithStack (const msgData& i_msgData, const char i_stack[][STACK_FRAME_BUF_SIZE], int i_stackSize) = 0;
      virtual void startBlock ()=0;
      virtual void writeError (const char* i_pErrorMessage)=0;
};

class fileLogFormatterWritter : public logMsgFormatterWriter
{

   public:
      fileLogFormatterWritter (FILE* i_outputFile) : m_outputFile(i_outputFile)
   {}

      virtual  void write (const msgData& i_msgData)
      {
         fprintf(m_outputFile, "Severity %d: Time %ld: ThreadID %u: FuncName %s: Message %s\n",
            i_msgData.m_severity, 
            i_msgData.m_time, 
            i_msgData.m_tid, 
            i_msgData.m_funcName, 
            i_msgData.m_msgText);
      }

      virtual void writeWithStack (const msgData& i_msgData, const char i_stack[][STACK_FRAME_BUF_SIZE], int i_stackSize)
      {
         write (i_msgData);
         fprintf(m_outputFile, "at:"); 

         //TODO: maybe skip trailing null frames?
         for (int i=0; i<i_stackSize; ++i)
         {
            fprintf(m_outputFile, "%s\n",i_stack[i]);
         }
      }

      virtual void startBlock ()
      {
         fprintf(m_outputFile, "*************************************************\n");
      }

      virtual void writeError (const char* i_pErrorMessage)
      {
         fprintf(m_outputFile, "%s\n",i_pErrorMessage);
      }

      virtual ~fileLogFormatterWritter(){}
   private:
      FILE* m_outputFile;
};

#endif
