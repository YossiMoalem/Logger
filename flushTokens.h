#ifndef FLUSH_TOKENS_H
#define FLUSH_TOKENS_H

#include <queue>
#include <pthread.h> 

class FlushTokens
{
   public:
   inline FlushTokens ();
   inline ~FlushTokens ();

   inline void addToken (msgTokenMngr::msg_token_t i_newToken);
   inline msgTokenMngr::msg_token_t getToken ();
   inline bool isEmpty () const;

   private:
   //TODO change to lock free queue
   std::queue<msgTokenMngr::msg_token_t>    m_queueFlushStartIndex;

   mutable pthread_mutex_t             m_startIndexMutex;

   /* Non-Copyable */ 
   FlushTokens (const FlushTokens&);
   FlushTokens& operator= (const FlushTokens&);
};

void FlushTokens::addToken(msgTokenMngr::msg_token_t i_newToken)
{
   pthread_mutex_lock(&m_startIndexMutex);

#if DEBUG >= 2
   PRINT_DEBUG ("Calling flush for identifier "<< i_newToken);
#endif

   m_queueFlushStartIndex.push(i_newToken);
   pthread_mutex_unlock(&m_startIndexMutex);
}

msgTokenMngr::msg_token_t  FlushTokens::getToken ()
{
   pthread_mutex_lock(&m_startIndexMutex);

   msgTokenMngr::msg_token_t  entryIdentifier = m_queueFlushStartIndex.front();
   m_queueFlushStartIndex.pop();

   pthread_mutex_unlock(&m_startIndexMutex);
#if DEBUG >= 2
         PRINT_DEBUG ("Popped identifier " <<entryIdentifier
               <<"(Index : " <<startIndex 
               <<"LifeID : " <<startLifeID <<")");
#endif
   return entryIdentifier;
}

bool FlushTokens::isEmpty() const
{
   bool isEmpty = false;
   pthread_mutex_lock(&m_startIndexMutex);
   isEmpty = m_queueFlushStartIndex.empty();
   pthread_mutex_unlock(&m_startIndexMutex);
   return isEmpty;
}
FlushTokens::FlushTokens ()
{
   pthread_mutexattr_t startIndexMutexAttr;
   pthread_mutexattr_settype(&startIndexMutexAttr, PTHREAD_MUTEX_ERRORCHECK); //(PTHREAD_MUTEX_RECURSIVE for prod??)
   pthread_mutex_init(&m_startIndexMutex, &startIndexMutexAttr);
}

FlushTokens::~FlushTokens ()
{
   pthread_mutex_destroy(&m_startIndexMutex);
}

#endif //FLUSH_TOKENS_H
