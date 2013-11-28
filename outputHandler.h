#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

class logMngr;

class outputHandler
{
   public:

       outputHandler(logMngr & i_logMngr);
      ~outputHandler();
      int doSomething();

   private:

   private:
      logMngr &m_logMngr;

};

#endif 

