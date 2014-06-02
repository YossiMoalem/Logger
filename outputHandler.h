#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

class outputHandler
{
   public:

      outputHandler();
      ~outputHandler();
      static void * startOutputWriterThread (void * i_logMngr);

   private:
      /* Non-Copyable */ 
      outputHandler (const outputHandler&);
      outputHandler& operator= (const outputHandler&);
};

#endif 

