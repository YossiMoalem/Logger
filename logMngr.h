#ifndef LOG_MNGR_H
#define LOG_MNGR_H

#include "logMsgEntity.h"
#include "outputHandler.h"
#include "entityIdentifierType.h"
#include <queue>
#include <pthread.h> 

#include <iostream>
#include <sstream>

#define NUM_OF_RECORDS_TO_FLUSH 20 
#define NUM_OF_LOG_MSGS  (NUM_OF_RECORDS_TO_FLUSH*10)

#define PRINT_DEBUG(MSG) do{\
std::stringstream ss;\
ss<< MSG <<std::endl;\
std::cerr << ss.str();\
}while(0);


class logMngr
{
	friend class outputHandler;

	public:

        logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter );
        ~logMngr();
		int write (const char *const  i_pMsgText, const char*const  i_pfuncName, time_t i_time, pid_t i_tid, int i_severity);
        void shutDown ();
      
   private:     

		int flushMessages (entityIdentifierType::entity_identifier_t i_entryIdentifier); 
        static void * startOutputWriterThread (void * i_logMngr);
        void startBlock();
        void writeError (const char* i_pErrorMessage);
     
   private:
        logMsgEntity                m_msgs[NUM_OF_LOG_MSGS] ;
		int 		                m_flushSeverity;
		logMsgFormatterWriter*      m_pLogMsgFormatterWriter;
        entityIdentifierType        m_entityIdentifierType;

         
         //TODO change to lock free queue
        std::queue<entityIdentifierType::entity_identifier_t>    m_queueFlushStartIndex;
        pthread_mutex_t             m_startIndexMutex;
        pthread_mutex_t             m_shutDownMutex;


};

 #endif
