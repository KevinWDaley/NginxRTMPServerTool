#include "/home/ireul/GitHub/NginxRTMPServerTool/src/nginx-process-id.h"

int main(int argc, char const *argv[]) {
  /* code */

  std::cout << "PID:" << nginxPID() << '\n';

  return 0;
}
