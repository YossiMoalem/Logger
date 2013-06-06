#include "logMngr.h"

void* writeToLog (void*args)
{
	// in loop, of how many messages to write:
		//get random number 0-100. if it is less than the flush percent - write it as FATAL. otherwise as info
		//write serial number as the function name
		//write variable length string as message text
		// Send it to the logger
}

int main ()
{
	// read how many threads to run
	// How many log each thread should write
	// and precents, who many flushing msgs 
	// create threads start func : writeToLog
	// run each one of them on deferent core
}
