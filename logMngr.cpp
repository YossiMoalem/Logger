#include "logMngr.h"
#include <iostream> // for std::cerr(debug)

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

int logMngr::write (const char *const  i_pMsgText, const char *const  i_pFunc, time_t i_time, pid_t i_pid, int i_severity)
{
#if 1 //TODO: only for debug:
   std::cerr <<i_time<<":"<<i_pid<<":"<<i_severity<<":"<<i_pFunc<<":"<<i_pMsgText<<"\n";
#endif 
	//TODO: implement!
}
