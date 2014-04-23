#ifndef LOG_MSG_FORMATTER_WRITER
#define LOG_MSG_FORMATTER_WRITER

#include <stdio.h>

class logMsgFormatterWriter
{
	public:
	virtual void write (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity)  = 0;
	virtual void writeWithStack (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,const char i_stack[][100],int i_stackSize)  = 0;
    virtual void startBlock ()=0;
    virtual void writeError (const char* i_pErrorMessage)=0;

};

class fileLogFormatterWritter : public logMsgFormatterWriter
{

   public:
      fileLogFormatterWritter (FILE* i_outputFile) : m_outputFile(i_outputFile)
   {}

      virtual  void write (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity) 
      {

         fprintf(m_outputFile, "Severity %d: Time %ld: ThreadID %d: FuncName %s: Message %s\n",i_severity, i_time, i_tid, i_pFuncName, i_pMsgText);

      }

	virtual void writeWithStack (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity,const char i_stack[][100], int i_stackSize)  
      {

         fprintf(m_outputFile, "Severity %d: Time %ld: ThreadID %d: FuncName %s: Message %s at \n",i_severity, i_time, i_tid, i_pFuncName, i_pMsgText);
 //TODO: maybe skip trailing null frames?
         for (int i=0;i<i_stackSize;++i)
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
   private:
      FILE* m_outputFile;
};

#endif
