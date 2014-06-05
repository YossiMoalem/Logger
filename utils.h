#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>

#ifndef DEBUG
#define DEBUG 0
#endif

#define PRINT_DEBUG(SEVERITY, MSG) do{\
if(SEVERITY <= DEBUG) {\
std::stringstream ss;\
ss<< MSG <<std::endl;\
std::cerr << ss.str();}\
}while(0);


#endif
