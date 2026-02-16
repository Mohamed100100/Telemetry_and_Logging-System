
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>

#include "services/TelemetryServiceImpl.hpp"



void TelemetryServiceImpl::getLoad(const std::shared_ptr<CommonAPI::ClientId> _client, getLoadReply_t _reply){
    _reply(GetCpuLoad());
}

uint8_t TelemetryServiceImpl::GetCpuLoad()
{
    static unsigned long long prevIdle = 0;
    static unsigned long long prevTotal = 0;

    std::ifstream file("/proc/stat");
    if (!file.is_open())
        return 0;

    std::string line;
    std::getline(file, line);   // first line: "cpu ..."

    std::istringstream ss(line);

    std::string cpu;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    unsigned long long idleTime = idle + iowait;
    unsigned long long totalTime = user + nice + system + idle + iowait + irq + softirq + steal;

    unsigned long long deltaIdle = idleTime - prevIdle;
    unsigned long long deltaTotal = totalTime - prevTotal;

    prevIdle = idleTime;
    prevTotal = totalTime;

    if (deltaTotal == 0)
        return 0;

    double cpuUsage = (1.0 - (double)deltaIdle / deltaTotal) * 100.0;

    return static_cast<uint8_t>(cpuUsage);
}

TelemetryServiceImpl::TelemetryServiceImpl(){
    // i just run it at the ctor because it will return at the first time 0 
    GetCpuLoad();
}
