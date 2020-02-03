#ifndef I_OSPARSER_H
#define I_OSPARSER_H

#include <unistd.h>

#include <string>
#include <vector>

class IOsParser {
 public:
  virtual ~IOsParser(){};
  // System
  virtual float MemoryUtilization() = 0;
  virtual long UpTime() = 0;
  virtual std::vector<int> Pids() = 0;
  virtual int TotalProcesses() = 0;
  virtual int RunningProcesses() = 0;
  virtual std::string OperatingSystem() = 0;
  virtual std::string Kernel() = 0;

  // CPU
  virtual std::vector<uint32_t> CpuUtilization() = 0;
  virtual long Jiffies() = 0;
  virtual long ActiveJiffies() = 0;
  virtual long ActiveJiffies(int pid) = 0;
  virtual long IdleJiffies() = 0;

  // Processes
  virtual std::string Command(int pid) = 0;
  virtual std::string Ram(int pid) = 0;
  virtual std::string Uid(int pid) = 0;
  virtual std::string User(int pid) = 0;
  virtual long int UpTime(int pid) = 0;
};
#endif