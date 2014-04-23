#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"
#include "msgTokenMngr.h"
#include <queue>
#include <pthread.h> 

#define NUM_OF_RECORDS_TO_FLUSH 20 
#define NUM_OF_LOG_MSGS  (NUM_OF_RECORDS_TO_FLUSH*10)


class logMngr
{
	friend class outputHandler;

	public:

        logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter );
        ~logMngr();
		int write (const char *const  i_pMsgText, const char*const  i_pfuncName, time_t i_time, pid_t i_tid, int i_severity,bool i_writeStack);
        void shutDown ();
      
   private:     

		int flushMessages (msgTokenMngr::msg_token_t i_entryIdentifier); 
        void startBlock();
        void writeError (const char* i_pErrorMessage);
     
   private:
        logMsgEntity                m_msgs[NUM_OF_LOG_MSGS] ;
		int 		                m_flushSeverity;
		logMsgFormatterWriter*      m_pLogMsgFormatterWriter;
        msgTokenMngr        m_msgTokenMngr;

         
         //TODO change to lock free queue
        std::queue<msgTokenMngr::msg_token_t>    m_queueFlushStartIndex;
        pthread_mutex_t             m_startIndexMutex;
        pthread_mutex_t             m_shutDownMutex;


};

 #endif
