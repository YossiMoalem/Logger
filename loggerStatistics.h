#ifndef LOGGER_STATISTICS_H
#define LOGGER_STATISTICS_H

/******************************************************************************\
 * Logger Statistics:
 * This class is used in order to monitor the operation of the logger, 
 * by collecting several statistics mesurments.
 * In order to activate this mechanism, STATISTICS flag should be used during compilation.
 *
 * Usage examle:
 *  loggerStatistics::instance().inc_counter(msgTokenMngr_failedAttempts);
 *
 * Adding new counter:
 *  add the new counter to "Counter" enum, and to "CounterDesc" array.
 *  Call inc_counter() and dec_counter() with the new counter, as needed.
 *
 * TODO:
 *  Think about the counter data type.
 ******************************************************************************/

#include <stdint.h>
#include <pthread.h>
#include <assert.h>

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
      static loggerStatistics* instance();

      /******************************************************************************\
       * Defines all the mesurements.
       * Naming conventions:
       *    moduleName_mesurement
       * Important!
       *    When adding counter, do not forget to add it also in CounterDesc array!
       ******************************************************************************/
      enum Counter
      {
         msgTokenMngr_failedAttempts = 0,
         logMsgEntity_setWhileInProgress,
         logMsgEntity_setWhileBeingFlushed,
         logMsgEntity_writeWhileBeingFlushed,
         outputWritter_CpuYield,
         outputWritter_overwrittenMsgs,
         counter_last //MUST be last!
      };



      /******************************************************************************\
       * This is the basic counter type.
       * TODO: Do we really need volataile ??
       * TODO: do we not want negative numbers??
       ******************************************************************************/
      typedef volatile uint64_t counter_t; 

      /******************************************************************************\
       * Increase the specified counter (i_counter) by 1
       ******************************************************************************/
      inline void inc_counter(Counter i_counter);

      /******************************************************************************\
       * Decrease the specified counter (i_counter) by 1
       ******************************************************************************/
      inline void dec_counter(Counter i_counter);

      /******************************************************************************\
       * Prints out the statistics via PRINT_DEBUG macro
       * called when the application terminates
       ******************************************************************************/
      void print();
   private:
      loggerStatistics();

      /* Non-Copyable */ 
      loggerStatistics (const loggerStatistics&);
      loggerStatistics& operator= (const loggerStatistics&);

   private:
      static loggerStatistics* s_instance;
      static pthread_mutex_t   m_creationLock;

      /******************************************************************************\
       * Holds all the counter values
       ******************************************************************************/
      __attribute__ ((aligned(8))) 
         counter_t m_countersValue[counter_last];
      /******************************************************************************\
       * Holds the counter descriptions. 
       * This is printed in the statistics summery.
       ******************************************************************************/
      static const char* const CounterDesc[counter_last] ;

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
