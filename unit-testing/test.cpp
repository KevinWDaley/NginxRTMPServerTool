#include <iostream>
#include "/home/ireul/Github/NginxRTMPServerTool/src/nginx-process-id.h"
#include "/home/ireul/Github/NginxRTMPServerTool/src/nginx-ip-address.h"
#include "/home/ireul/Github/NginxRTMPServerTool/src/ngnix-memory.h"
#include "/home/ireul/Github/NginxRTMPServerTool/src/ngnix-cpu-load.h"

size_t g_previous_idle_time=0, g_previous_total_time=0;

int main(int argc, char const *argv[]) {

//  size_t g_previous_idle_time=0, g_previous_total_time=0;

  std::cout << "UNIT TESTING" << '\n';
  std::cout << "Function nginxPID: " << "\"" << nginxPID() << "\"" << '\n';
  std::cout << "Function getIpAddress: " << "\"" << getIpAddress() << "\"" << '\n';
  std::cout << "Function memoryUsage: " << "\"" << memoryUsage() << "\"GB" << '\n';
  std::cout << "Function cpuLoad:" << "\"" << cpuLoad() << "%\"" << '\n';
  std::cout << "Function cpuLoad:" << "\"" << cpuLoad() << "\"%" << '\n';
  std::cout << "Function cpuLoad:" << "\"" << cpuLoad() << "\"%" << '\n';
/*
  for (int i = 0; i < 5; i++) {
    std::cout << "Function cpuLoad:" << "\"" << cpuLoad() << "\"%" << '\n';
  }
*/
  std::cout << "Function :" << "\"" << "" << "\"" << '\n';
  std::cout << "Function :" << "\"" << "" << "\"" << '\n';
  std::cout << "Function :" << "\"" << "" << "\"" << '\n';

  return 0;
}
