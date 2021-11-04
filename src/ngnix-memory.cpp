#include "ngnix-memory.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

float memoryUsage()
{

    std::vector<std::string> memory;
    std::string str;
    std::ifstream proc_mem("/proc/meminfo");
    for( std::string str; proc_mem >> str; memory.push_back(str));
    size_t memTotal;
    size_t memAvailable;
    float memUsed;
    std::stringstream sstream(memory[1]);
    std::stringstream ssstream(memory[7]);
    sstream >> memTotal;
    ssstream >> memAvailable;
    memUsed = ((float)(memTotal - memAvailable)) / 1000000;
    return memUsed;

    return 0;

}
