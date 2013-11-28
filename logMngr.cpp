#include "logMngr.h"
#include <iostream> // for std::cerr(debug)
#include <iomanip> //for std::setw
#include <sstream>

#define WORD_SIZE 32 
#define WORD_MAX_VAL 0xFFFFFFFF
#define GET_CUR_INDEX(ENTRY_IDENT)  (((EntryIdentifierType) (ENTRY_IDENT)) & WORD_MAX_VAL)
#define GET_CUR_LIFE_ID(ENTRY_IDENT) (((EntryIdentifierType) (ENTRY_IDENT)) >> WORD_SIZE)
#define CREATE_ENTRY_IDENT(LIFE_ID,INDEX) ((((EntryIdentifierType) (LIFE_ID)) << WORD_SIZE) | (INDEX & WORD_MAX_VAL))

//static_assert (NUM_OF_LOG_MSGS<0xFFFFFFFF);
//static_assert (sizeof(EntryIdentifierType)>=8);

//==================================================================================================================================================
logMngr::logMngr (int i_flushSeverity,  logMsgFormatterWriter* i_pLogMsgFormatterWriter ) : 
   m_curEntryIndent(CREATE_ENTRY_IDENT(1,0)),
   m_flushSeverity(i_flushSeverity),
   m_pLogMsgFormatterWriter(i_pLogMsgFormatterWriter)
{
   pthread_mutex_init(&m_startIndexMutex, NULL);
   pthread_create(&m_outputWriter,NULL,&startOutputWriterThread,this);
}

//==================================================================================================================================================
logMngr::~logMngr()
{
   pthread_mutex_destroy(&m_startIndexMutex);
}


//==================================================================================================================================================
void logMngr::getNextIndex (unsigned int &o_index,unsigned int& o_lifeID) 
{ 
   EntryIdentifierType newVal  = 0;
   EntryIdentifierType oldVal  = 0;
   do 
   {
      oldVal = m_curEntryIndent ;
      o_index  = GET_CUR_INDEX (oldVal);
      o_lifeID = GET_CUR_LIFE_ID (oldVal);
      o_index  = (o_index + 1) % NUM_OF_LOG_MSGS;
      if (0 == o_index)
      {   
         o_lifeID++;
      }
      newVal = CREATE_ENTRY_IDENT (o_lifeID,o_index);
   } while (! __sync_bool_compare_and_swap (&m_curEntryIndent, oldVal, newVal));
}


//==================================================================================================================================================
int logMngr::flushMessages (unsigned int i_curIndex)
{

   pthread_mutex_lock(&m_startIndexMutex);
   m_queueFlushStartIndex.push(i_curIndex);
   pthread_mutex_unlock(&m_startIndexMutex);
}


//==================================================================================================================================================
int logMngr::write (const char *const  i_pMsgText, const char *const  i_pFunc, time_t i_time, pid_t i_pid, int i_severity)
{
   unsigned int curIndex;
   unsigned int curLifeID;
   
   this->getNextIndex(curIndex,curLifeID);

#if 1 //TODO: only for debug:
   std::stringstream ss;
   ss<< curLifeID<<std::setw(2)<<std::setfill('0')<< curIndex;

   ss <<" YOSSI current Index "<<curIndex<< " severity "<<i_severity<<" Time "<<i_time<<" ThreadID "<<i_pid << " FuncName "<< i_pFunc<<" current lifeID "<< curLifeID<<std::endl;
   std::cerr << ss.str() ;
#endif 
   int retval = 0;
   m_msgs[curIndex].set(i_pMsgText,i_pFunc,i_time,i_pid,i_severity,curLifeID);
   if (i_severity >= m_flushSeverity)
   {
      retval = this->flushMessages(curIndex);
   }

   return retval;
}


//==================================================================================================================================================
void * logMngr::startOutputWriterThread (void * i_logMngr)
{
   logMngr *currLogMngr = static_cast <logMngr*> (i_logMngr);
   outputHandler *pOutputHandler = new outputHandler(*currLogMngr);
   pOutputHandler->doSomething();

}

//==================================================================================================================================================
void logMngr::startBlock()
{
   m_pLogMsgFormatterWriter->startBlock();
}
 
//==================================================================================================================================================
void logMngr::writeError (const char* i_pErrorMessage)
{
   m_pLogMsgFormatterWriter->writeError(i_pErrorMessage);
}
