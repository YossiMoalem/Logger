#ifndef FLUSH_TOKENS_H
#define FLUSH_TOKENS_H

#include <queue>
#include <pthread.h> 

/******************************************************************************\
 *
 * This holds the tokens of messages that initiate flush.
 * Each message that should initiate flush, adds it's token here (using addToken) 
 * (from which we can extract eh cell ID, and the LID),
 * ans the output handler should pop them(using getToken), one at a time, and start the flush
 *
 * TODO: Concider using lock free queue
 ******************************************************************************/

class FlushTokensHolder
{
   public:
   inline FlushTokensHolder ();
   inline ~FlushTokensHolder ();

   inline void addToken (msgTokenMngr::msg_token_t i_newToken);
   inline msgTokenMngr::msg_token_t getToken ();
   inline bool isEmpty () const;

   private:
   //TODO change to lock free queue
   std::queue<msgTokenMngr::msg_token_t>    m_queueFlushStartIndex;

   mutable pthread_mutex_t             m_startIndexMutex;

   /* Non-Copyable */ 
   FlushTokensHolder (const FlushTokensHolder&);
   FlushTokensHolder& operator= (const FlushTokensHolder&);
};

/******************************************************************************/
FlushTokensHolder::FlushTokensHolder ()
{
   pthread_mutexattr_t startIndexMutexAttr;
   pthread_mutexattr_settype(&startIndexMutexAttr, PTHREAD_MUTEX_ERRORCHECK); //(PTHREAD_MUTEX_RECURSIVE for prod??)
   pthread_mutex_init(&m_startIndexMutex, &startIndexMutexAttr);
}

/******************************************************************************/
void FlushTokensHolder::addToken(msgTokenMngr::msg_token_t i_newToken)
{
   pthread_mutex_lock(&m_startIndexMutex);

   PRINT_DEBUG (2, "Calling flush for identifier "<< i_newToken);

   m_queueFlushStartIndex.push(i_newToken);
   pthread_mutex_unlock(&m_startIndexMutex);
}

/******************************************************************************/
msgTokenMngr::msg_token_t  FlushTokensHolder::getToken ()
{
   pthread_mutex_lock(&m_startIndexMutex);

   msgTokenMngr::msg_token_t  entryIdentifier = m_queueFlushStartIndex.front();
   m_queueFlushStartIndex.pop();

   pthread_mutex_unlock(&m_startIndexMutex);
   return entryIdentifier;
}

/******************************************************************************/
bool FlushTokensHolder::isEmpty() const
{
   bool isEmpty = false;
   pthread_mutex_lock(&m_startIndexMutex);
   isEmpty = m_queueFlushStartIndex.empty();
   pthread_mutex_unlock(&m_startIndexMutex);
   return isEmpty;
}

FlushTokensHolder::~FlushTokensHolder ()
{
   pthread_mutex_destroy(&m_startIndexMutex);
}

#endif //FLUSH_TOKENS_H
