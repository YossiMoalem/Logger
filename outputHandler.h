#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

class logMngr;

class outputHandler
{
   public:

       outputHandler();
      ~outputHandler();
        static void * startOutputWriterThread (void * i_logMngr);


};

#endif 

