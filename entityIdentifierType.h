
#ifndef ENTITY_IDENTIFIER_TYPE
#define ENTITY_IDENTIFIER_TYPE

#define WORD_SIZE 32 
#define WORD_MAX_VAL 0xFFFFFFFF
#define GET_CUR_INDEX(ENTRY_IDENT)  (((entityIdentifierType::entity_identifier_t) (ENTRY_IDENT)) & WORD_MAX_VAL)
#define GET_CUR_LIFE_ID(ENTRY_IDENT) (((entityIdentifierType::entity_identifier_t) (ENTRY_IDENT)) >> WORD_SIZE)
#define CREATE_ENTRY_IDENT(LIFE_ID,INDEX) ((((entityIdentifierType::entity_identifier_t) (LIFE_ID)) << WORD_SIZE) | (INDEX & WORD_MAX_VAL))
#define SHUTDOWN_ENTRY (CREATE_ENTRY_IDENT(0,0))
#define IS_SHUTDOWN_IDENT(IDENT) ((entityIdentifierType::entity_identifier_t) (IDENT)  == 0) 



//static_assert (sizeof(entryIdentifierType::entity_identifier_t)>=8);

class entityIdentifierType
{
 		public:
        typedef unsigned long long entity_identifier_t ;       
        
        entityIdentifierType(unsigned int i_numOfMsgs): m_numOfMsgs(i_numOfMsgs),
                                                      m_curEntryIndent(CREATE_ENTRY_IDENT(1,0))
                                                      {}
        
        void getNextIndex (unsigned int &o_index,unsigned int& o_lifeID,entity_identifier_t &o_entryIdentifier); 

      private:

      unsigned int m_numOfMsgs;
        entity_identifier_t         m_curEntryIndent;
};



inline void entityIdentifierType::getNextIndex (unsigned int &o_index,unsigned int& o_lifeID,entity_identifier_t &o_entryIdentifier) 
{ 
   entity_identifier_t oldVal  = 0;
   do 
   {
      oldVal = m_curEntryIndent ;
      o_index  = GET_CUR_INDEX (oldVal);
      o_lifeID = GET_CUR_LIFE_ID (oldVal);
      o_index  = (o_index + 1) % m_numOfMsgs;
      if (0 == o_index)
      {   
         o_lifeID++;
      }
      o_entryIdentifier = CREATE_ENTRY_IDENT (o_lifeID,o_index);
   } while (! __sync_bool_compare_and_swap (&m_curEntryIndent, oldVal, o_entryIdentifier));
}



#endif
