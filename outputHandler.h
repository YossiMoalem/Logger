#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

/******************************************************************************\
 *
 * This class is responsible for poping flash token from the flushTokenHolder
 * and flushing the required number of messages from this index backwords.
 *
 * It loops untill it gets shutdown message, and everytime there is 
 * a flush token available - it ftart the flushing.
 *
 * It runs on a dedicated thread, created by the logMngr
 ******************************************************************************/
template <class Writer>
class outputHandler
{
   public:
      outputHandler()
      {}

      /* Entry point for the created thread */
      static void * startOutputWriterThread (void * i_logMngr);

   private:
      /* Non-Copyable */ 
      outputHandler (const outputHandler&);
      outputHandler& operator= (const outputHandler&);
};

#include "outputHandler.hpp"

#endif 

