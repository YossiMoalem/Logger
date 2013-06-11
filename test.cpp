# include <omp.h>       //for omp_get_thread_num
// ** Compile with -fopenmp  ** \\

#include <iostream>     //for cout
#include <stdlib.h>     //for atoi, srand, rand
#include <time.h>       //for time

#include "logMngr.h"
#include "logMsgFormatterWriter.h"

void printHelp (const char i_pExeName[])
{
   std::cout <<"Usage: " <<i_pExeName <<" <Number of threads> <num of messages > <precentage of flush messages(this will also be set as the flush severity)> \n";
}

void randString (int i_buffSize, char o_buff[])
{
   static const char charset[] = "abcdefghijklmnopQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-=!@#$%^&*()<>?/.\\ o`~\"";
   int len = rand() % i_buffSize;
   for (int i = 0; i < len; ++i)
   {
      int index = rand() % (sizeof (charset) / sizeof(char));
      o_buff[i] = charset[index];
   }
   o_buff[len] = 0;
}
int main (int argc, char* argv[])
{
   if ( argc != 4 )
   {
      printHelp (argv[0]);
      return -1;
   } else {
      int numOfThreads = atoi (argv[1]);
      int numOfMessages = atoi (argv[2]);
      int flushMessagesPrecent = atoi (argv[3]);
      if (numOfThreads <= 0 || numOfMessages <= 0 || flushMessagesPrecent <= 0 )
      {
         printHelp(argv[0]);
         return -1;
      }
      std::cout << "Going to create "<< numOfThreads <<" Threads. They will send "<< numOfMessages << " messages, " <<flushMessagesPrecent <<" percent of them will require flush \n";
      // init logger
      fileLogFormatterWritter* pLogWriter = new fileLogFormatterWritter(stdout); 
      logMngr* pLogger = new logMngr (flushMessagesPrecent, pLogWriter); 

      #pragma omp parallel num_threads(3)
      #pragma omp for
      for (int i = 0; i < numOfMessages; ++i )
      {
         //   srand (time(NULL));  // We do not want srand, so we can repeat the same test...
         int severity = 100- (rand () % 100);
         //TODO: change message text and function to something more interesting...
         char message[200];
         randString(200, message);

         char messageID [100] = {0};
         snprintf (messageID, 100, "Message %d", i);

         pLogger->write (message, messageID, time(NULL),  (pid_t)omp_get_thread_num(), severity);
      }

   }
}
