#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, IOsParser* const parser)
    : pid_(pid),
      p_parser_(parser),
      ram_(0),
      prev_cputotaltime_(0),
      prev_totaltime_(0),
      cpu_util_(Process::CpuUtilization()) {}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  float totaltime = p_parser_->ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  float uptime = p_parser_->UpTime(pid_);
  prev_totaltime_ = totaltime;
  return totaltime / uptime;
}

// DONE: Return the command that generated this process
string Process::Command() { return p_parser_->Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return p_parser_->Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return p_parser_->User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return p_parser_->UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return cpu_util_ < a.cpu_util_;
}

bool Process::operator>(Process const& a) const {
  return cpu_util_ > a.cpu_util_;
}

uint* Process::GetPrevCpuTotal() { return &prev_cputotaltime_; }

ulong* Process::GetPrevTotaltime() { return &prev_totaltime_; }