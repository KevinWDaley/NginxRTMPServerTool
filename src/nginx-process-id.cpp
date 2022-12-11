#include "nginx-process-id.h"
#include <fstream>

bool NginxPid()
{
  std::ifstream conf;
	conf.open( "/usr/local/nginx/logs/nginx.pid");
	return conf.is_open();
}
