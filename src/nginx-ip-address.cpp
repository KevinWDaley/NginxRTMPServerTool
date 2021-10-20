#include "nginx-ip-address.h"
#include <iostream>
#include <fstream>
#include <string>

std::string getIpAddress()
{
    char temp;
    char ipAddress[32] = {'\0'};
    FILE* pipe = popen("hostname -I","r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    int i = 0;
    while (!feof(pipe))
    {

      temp = fgetc(pipe);
      if( temp == '0' || temp == '1' || temp == '2' || temp == '3' || temp == '4' || temp == '5' || temp == '6' || temp == '7' || temp == '8' || temp == '9' || temp == '.' )
      {
        ipAddress[i++] = temp;
      }

    }

    pclose(pipe);
    return ipAddress;
}
