#ifndef NGINX_CPU_LOAD_H
#define NGINX_CPU_LOAD_H

#include <vector>
#include <fstream>

extern size_t g_previous_idle_time, g_previous_total_time;

float cpuLoad();

#endif
