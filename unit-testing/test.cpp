#include "/home/ireul/Github/NginxRTMPServerTool/src/nginx-process-id.h"

int main(int argc, char const *argv[]) {
  /* code */

  std::cout << "PID:" << nginxPID() << '\n';

  return 0;
}
