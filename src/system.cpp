#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// constructor
System::System() : p_parser_(new LinuxParser()), cpu_(p_parser_) {}

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = p_parser_->Pids();
  processes_ = {};
  for (auto const& pid : pids) {
    // create a Process obj and pass to the vector if process doesn't exist
    processes_.emplace_back(pid, p_parser_);
  }
  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return p_parser_->Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return p_parser_->MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return p_parser_->OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return p_parser_->RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return p_parser_->TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return p_parser_->UpTime(); }