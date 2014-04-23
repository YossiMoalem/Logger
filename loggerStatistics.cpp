#include "loggerStatistics.h"


loggerStatistics*  loggerStatistics::s_instance = NULL;

const char* const loggerStatistics::CounterDesc[] = {
   "Token Manager : Number of failed attempts to create Token                     ",
   "logMsgEntity  : Number of attempts to set msg while it was being set          ",
   "logMsgEntity  : Number of attempts to set msg while it was being flushed      ",
   "logMsgEntity  : Number of attempts to write  msg while it was being flushed   ",
   "outputWritter : Number of CPU Yield (k) due to wait for message to be written ",
   "outputWritter : Number of overwritten messages                                ",
   };

loggerStatistics::loggerStatistics ()
{
   for (int i = 0; i < counter_last; ++i)
       m_countersValue[i] = 0;
}
void loggerStatistics::print ()
{
#ifdef STATISTICS
   PRINT_DEBUG ("******* Statistics **********");
   for (int i = 0; i < counter_last; ++i)
      PRINT_DEBUG(CounterDesc[i]<<  " : " << m_countersValue[i] );
#endif
}
