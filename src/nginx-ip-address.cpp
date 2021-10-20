#include "nginx-ip-address.h"
#include <fstream>
#include <string>

string getIpAddress()
{
    char buffer[15] = {'\0'};
    char ipAddress[15] = {'\0'};
    shared_ptr<FILE> pipe(popen("hostname -I","r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");

    int i = 0;
  while (!feof(pipe.get()))
  {
      buffer[i++] = fgetc(pipe.get());
  }
  //buffer[14] = '+';

  for( int i = 0; i < 15; i++)
  {
      if( buffer[i] == '1' || buffer[i] == '2' || buffer[i] == '3' || buffer[i] == '4' || buffer[i] == '5' || buffer[i] == '6' || buffer[i] == '7' || buffer[i] == '8' || buffer[i] == '9' || buffer[i] == '.' )
      {
          ipAddress[i] = buffer[i];
      }

  }

  return ipAddress;
}
