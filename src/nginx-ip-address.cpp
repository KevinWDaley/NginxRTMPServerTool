#include "nginx-ip-address.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using std::string;

string getIpAddress()
{
    char buffer[15];
    string ipAddress = "";
    FILE* pipe = popen("hostname -I","r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe))
    {
        if ( fgets(buffer, 15, pipe) != NULL )
        {
          ipAddress +=buffer;
        }
    }

    pclose(pipe);
    return ipAddress;
}
