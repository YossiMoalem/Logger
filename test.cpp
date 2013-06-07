#include <iostream> //for cout
#include <stdlib.h> //for atoi, srand, rand
#include <time.h>  //for time
#include "logMngr.h"
#include "logMsgFormatterWriter.h"

int numOfThreads = 0;
int messagesPerThread = 0;
int flushMessagesPrecent = 0;

logMngr* pLogger = 0;

void* writeToLog (void* args)
{
   pid_t tid = pthread_self();
   for (int i = 0; i < messagesPerThread; ++i )
   {
   //   srand (time(NULL));  // We do not want srand, so we can repeat the same test...
      int severity = 100- (rand () % 100);
      //TODO: change message text and function to something more interesting...
      //TODO: add some king of tacing on messages (id)
      pLogger->write ("Message Text","Thisfunctionname", time(NULL),  tid, severity);
   }
}

void printHelp (const char i_pExeName[])
{
   std::cout <<"Usage: " <<i_pExeName <<" <Number of threads> <How many messages per threads> <precentage of flush messages(this will also be set as the flush severity)> \n";
}

int main (int argc, char* argv[])
{
   if ( argc != 4 )
   {
      printHelp (argv[0]);
      return -1;
   } else {
      numOfThreads = atoi (argv[1]);
      messagesPerThread = atoi (argv[2]);
      flushMessagesPrecent = atoi (argv[3]);
      if (numOfThreads <= 0 || messagesPerThread <= 0 || flushMessagesPrecent <= 0 )
      {
         printHelp(argv[0]);
         return -1;
      }
      std::cout << "Going to create "<< numOfThreads <<" Threads. Each one will send "<< messagesPerThread << " messages, " <<flushMessagesPrecent <<"percent of them will require flush \n";
      // init logger
      fileLogFormatterWritter* pLogWriter = new fileLogFormatterWritter(stdout); 
      pLogger = new logMngr (flushMessagesPrecent, pLogWriter); 

      // create threads start func 
      pthread_t t[numOfThreads] ; 
      for(int i = 0; i < numOfThreads; ++i)
      {
         //TODO: retval, args, cores...
         pthread_create (&t[i], 0, writeToLog, 0);
      }
      for(int i = 0; i < numOfThreads; ++i)
      {
         //TODO: retval
         void* ret;
         pthread_join(t[i], &ret);
      }

   }
}
