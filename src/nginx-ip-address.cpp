#include "nginx-ip-address.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using std::string;

string getIpAddress()
{
    char buffer[15] = {'\0'};
    char ipAddress[15] = {'\0'};
    std::shared_ptr<FILE> pipe(popen("hostname -I","r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    int i = 0;
    while (!feof(pipe.get()))
    {
        ipAddress[i] = fgetc(pipe.get());
    }

    return ipAddress;
}
