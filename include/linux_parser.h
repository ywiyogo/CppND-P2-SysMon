#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

#include "i_osparser.h"

class LinuxParser : public IOsParser {
 public:
  LinuxParser();
  virtual ~LinuxParser();
  // Paths
  const std::string kProcDirectory{"/proc/"};
  const std::string kCmdlineFilename{"/cmdline"};
  const std::string kCpuinfoFilename{"/cpuinfo"};
  const std::string kStatusFilename{"/status"};
  const std::string kStatFilename{"/stat"};
  const std::string kUptimeFilename{"/uptime"};
  const std::string kMeminfoFilename{"/meminfo"};
  const std::string kVersionFilename{"/version"};
  const std::string kOSPath{"/etc/os-release"};
  const std::string kPasswordPath{"/etc/passwd"};

  // System
  virtual float MemoryUtilization();
  virtual long UpTime();
  virtual std::vector<int> Pids();
  virtual int TotalProcesses();
  virtual int RunningProcesses();
  virtual std::string OperatingSystem();
  virtual std::string Kernel();

  // CPU
  enum CPUStates {
    kUser_ = 0,
    kNice_,
    kSystem_,
    kIdle_,
    kIOwait_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
  };
  virtual std::vector<std::uint32_t> CpuUtilization();
  virtual long Jiffies();
  virtual long ActiveJiffies();
  virtual long ActiveJiffies(int pid);
  virtual long IdleJiffies();

  // Processes
  virtual std::string Command(int pid);
  virtual std::string Ram(int pid);
  virtual std::string Uid(int pid);
  virtual std::string User(int pid);
  virtual long int UpTime(int pid);

  private:
  void AddOffsetSpace(std::string& text, int n);
};  // Class LinuxParser

#endif