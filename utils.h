#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>

#define PRINT_DEBUG(MSG) do{\
std::stringstream ss;\
ss<< MSG <<std::endl;\
std::cerr << ss.str();\
}while(0);


#endif
