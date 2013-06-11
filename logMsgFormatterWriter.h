#ifndef LOG_MSG_FORMATTER_WRITER
#define LOG_MSG_FORMATTER_WRITER

#include <stdio.h> //for fprintf

class logMsgFormatterWriter
{
	public:
	virtual void write (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity)  = 0;
};

class fileLogFormatterWritter : public logMsgFormatterWriter
{

	public:
	fileLogFormatterWritter (FILE* i_outputFile) : m_outputFile(i_outputFile)
	{}

	virtual  void write (const char* i_pMsgText, const char* i_pFuncName, time_t i_time, pid_t i_tid, int i_severity) 
	{

		fprintf(m_outputFile, "%d:%ld:%d:%s:%s",i_severity, i_time, i_tid, i_pFuncName, i_pMsgText);
	}

        private:
        FILE* m_outputFile;
};

#endif
