#include "ngnix-cpu-load.h"
#include <vector>
#include <fstream>
#include <numeric>

float cpuLoad()
{
    std::vector<size_t> cpu;
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix
    for (size_t time; proc_stat >> time; cpu.push_back(time));
    size_t idle_time = cpu[3];
    size_t total_time = accumulate(cpu.begin(), cpu.end(), 0);
    float idle_time_delta = idle_time - g_previous_idle_time;
    float total_time_delta = total_time - g_previous_total_time;
    float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
    g_previous_idle_time = idle_time;
    g_previous_total_time = total_time;

    return utilization;
}
