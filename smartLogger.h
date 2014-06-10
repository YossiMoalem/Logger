#ifndef SMART_LOGGER_H
#define SMART_LOGGER_H

#include "logMngr.h"

/******************************************************************************\
 * SmartLogger:
 * This provides a set of macros to easily use the logger:
 * First, obtain a logger handle:
 *     logMngr* pLogger = GET_LOGGER(90, pLogWriter); 
 * Now you can log messages:
 *      LOG_MSG(pLogger, "My Text", 20);
 *Finally, wen the  application terminates, shutdown the logger:
      SHUTDOWN_LOGGER(pLogger);
 *
 ******************************************************************************/

#define GET_LOGGER(Severity, pWriter) \
         new logMngr<pWriter>(Severity);

#define LOG_MSG(pLogger, Text, Severity) \
               pLogger->write(Text, __PRETTY_FUNCTION__, time(0), pthread_self(), Severity, false);  

#define LOG_MSG_WITH_STACK (pLogger, Text, Severity) \
               pLogger->write(Text, __PRETTY_FUNCTION__, time(0), pthread_self(), Severity, true);  

#define SHUTDOWN_LOGGER(pLogger) \
               pLogger->shutDown();

#endif//SMART_LOGGER_H

