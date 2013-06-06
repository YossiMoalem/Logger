#include "logMngr.h"

unsigned int logMngr::getNextIndex () 
{ 
	int oldVal = 0;
	int newVal = 0;
	do 
	{
		oldVal = m_curIndex;
		newVal = (m_curIndex + 1) % NUM_OF_LOG_MSGS;
	} while ( __sync_bool_compare_and_swap (&m_curIndex, oldVal, newVal));
	return m_curIndex;
}
int logMngr::flushMessages (unsigned int i_curIndex)
{
	//TODO: implement!
}

int logMngr::write (const char* i_pMsgText, const char* i_pfunc, time_t i_time, pid_t i_pid, int i_severity)
{
	//TODO: implement!
}
