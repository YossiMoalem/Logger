#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"
#include "outputHandler.h"
#include <queue>
#include <pthread.h> 

#define NUM_OF_RECORDS_TO_FLUSH 20 
#define NUM_OF_LOG_MSGS  (NUM_OF_RECORDS_TO_FLUSH*4)
class logMngr
{
	friend class outputHandler;

	public:

        logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter );
        ~logMngr();
		int write (const char *const  i_pMsgText, const char*const  i_pfunc, time_t i_time, pid_t i_pid, int i_severity);
        void startBlock();
        void writeError (const char* i_pErrorMessage);

      
   private:     
		typedef unsigned long long EntryIdentifierType ;
		
        void getNextIndex (unsigned int &o_index,unsigned int& o_lifeID); 
		int flushMessages (unsigned int i_curIndex);
        static void * startOutputWriterThread (void * i_logMngr);
     
   private:
        logMsgEntity                m_msgs[NUM_OF_LOG_MSGS] ;
        EntryIdentifierType         m_curEntryIndent;
		int 		                m_flushSeverity;
		logMsgFormatterWriter*      m_pLogMsgFormatterWriter;
        pthread_t                   m_outputWriter;
         
         //TODO change to lock free queue
        std::queue<unsigned int>    m_queueFlushStartIndex;
        pthread_mutex_t             m_startIndexMutex;

};

#endif
