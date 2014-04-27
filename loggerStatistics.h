#ifndef LOGGER_STATISTICS_H
#define LOGGER_STATISTICS_H

#include <stdint.h>
#include "assert.h"
#include "utils.h"

#ifdef STATISTICS
#define ATOMIC_INC(X) asm volatile ("lock ;incl %0"\
                                    : "+m" (X));

#define ATOMIC_DEC(X) asm volatile ("lock ;decl %0"\
                                    : "+m" (X));
#else
//Suppress the unused variable warning
#define ATOMIC_INC(X) (void)(X)
#define ATOMIC_DEC(X) (void)(X)
#endif

class loggerStatistics
{
   public:
      static loggerStatistics* instance()
      {
         if (s_instance == NULL)
         assert (0);
         return s_instance;
      }

      static void create()
      {
         if (s_instance == NULL)
            s_instance = new loggerStatistics;
         else
            assert (0);
      }

      enum Counter
      {
         msgTokenMngr_failedAttempts = 0,
         logMsgEntity_setWhileInProgress,
         logMsgEntity_setWhileBeingFlushed,
         logMsgEntity_writeWhileBeingFlushed,
         outputWritter_CpuYield,
         outputWritter_overwrittenMsgs,
         counter_last
      };



      //TODO: Do we really need volataile ??
      typedef volatile uint64_t counter_t; 

      inline void inc_counter(Counter i_counter);
      inline void dec_counter(Counter i_counter);

      void print();
   private:
      loggerStatistics();

   private:
      static loggerStatistics* s_instance;

      __attribute__ ((aligned(8))) 
      static const char* const CounterDesc[counter_last] ;
      counter_t m_countersValue[counter_last];

};

void loggerStatistics::inc_counter(Counter i_counter)
{
   ATOMIC_INC(m_countersValue[i_counter]);
}

void loggerStatistics::dec_counter(Counter i_counter)
{
   ATOMIC_DEC(m_countersValue[i_counter]);
}


#undef ATOMIC_INC
#undef ATOMIC_DEC

#endif
