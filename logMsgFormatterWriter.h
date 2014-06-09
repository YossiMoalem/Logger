#ifndef LOG_MSG_FORMATTER_WRITER
#define LOG_MSG_FORMATTER_WRITER

#include "msgData.h"

#include <stdio.h>

/******************************************************************************\
 * This class defines the minimal intf thats needed to be implemented by any writer
 * Those are teh functions that the logger will call.
 * Other functions (e.g. init() ) can be added.
 * Those function can be called using the logMngr getWriter() method
 ******************************************************************************/
class logMsgFormatterWriter
{
   public:
      void write (const msgData& i_msgData);
      void writeWithStack (const msgData& i_msgData, 
                            const char i_stack[][STACK_FRAME_BUF_SIZE], 
                            int i_stackSize);
      void startBlock ();
      void writeError (const char* i_pErrorMessage);
};

/******************************************************************************\
 * Simple writter that writes the lof to file-descriptor.
 ******************************************************************************/
class fileLogFormatterWritter : public logMsgFormatterWriter
{
   public:
      fileLogFormatterWritter () : m_outputFile(stdout)
      {}

      void setFd (FILE* i_outputFile)
      { 
         m_outputFile = i_outputFile;
      }

      void write (const msgData& i_msgData)
      {
         fprintf(m_outputFile, "Severity %d: Time %ld: ThreadID %u: FuncName %s: Message %s\n",
               i_msgData.m_severity, 
               i_msgData.m_time, 
               i_msgData.m_tid, 
               i_msgData.m_funcName, 
               i_msgData.m_msgText);
      }

      void writeWithStack (const msgData& i_msgData, 
            const char i_stack[][STACK_FRAME_BUF_SIZE], 
            int i_stackSize)
      {
         write (i_msgData);
         fprintf(m_outputFile, "at:"); 

         //TODO: maybe skip trailing null frames?
         for (int i=0; i<i_stackSize; ++i)
         {
            fprintf(m_outputFile, "%s\n",i_stack[i]);
         }
      }

      void startBlock ()
      {
         fprintf(m_outputFile, "*************************************************\n");
      }

      void writeError (const char* i_pErrorMessage)
      {
         fprintf(m_outputFile, "%s\n",i_pErrorMessage);
      }

      ~fileLogFormatterWritter(){}
   private:
      FILE* m_outputFile;
};

#endif
