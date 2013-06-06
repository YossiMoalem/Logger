#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"


#define NUM_OF_LOG_MSGS  20
class logMngr
{
	private:
		logMsgEntity m_msgs[NUM_OF_LOG_MSGS] ;
		unsigned int 	m_curIndex;
		int 		m_flushSeverity;

		unsigned int getNextIndex (); 
		int flushMessages (unsigned int i_curIndex);
		logMsgFormatterWriter* m_pLogMsgFormatterWriter;

	public:
		logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter ) : 
			m_flushSeverity(i_flushSeverity),
			m_pLogMsgFormatterWriter(i_pLogMsgFormatterWriter)
		 {}

		int write (const char* i_pMsgText, const char* i_pfunc, time_t i_time, pid_t i_pid, int i_severity);
};

#endif
