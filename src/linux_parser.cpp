#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace fs = std::experimental::filesystem;

// constructor
LinuxParser::LinuxParser() {}
// Destructor
LinuxParser::~LinuxParser() {}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    string line;
    string key;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  std::ifstream stream(kProcDirectory + kVersionFilename);
  string kernel;
  if (stream.is_open()) {
    string os;
    string version;

    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  if (fs::exists(kProcDirectory.c_str()) &&
      fs::is_directory(kProcDirectory.c_str())) {
    for (const auto& entry : fs::directory_iterator(kProcDirectory.c_str())) {
      if (fs::is_directory(entry.status())) {
        string filename = entry.path().filename();
        if (std::all_of(filename.begin(), filename.end(), isdigit)) {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
  }
  return pids;
}

// DONE: Read and return the system memory utilization
// Ref:
// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
// Total used memory = MemTotal - MemFree
// Non cache/buffer memory (green) = Total used memory - (Buffers + Cached
// memory) Buffers (blue) = Buffers Cached memory (yellow) = Cached +
// SReclaimable - Shmem Swap = SwapTotal - SwapFree
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  uint mem_total = 0;
  uint mem_free = 0;
  uint buffers = 0;
  uint cached = 0;
  uint shmem = 0;
  uint sreclaimable = 0;
  if (filestream.is_open()) {
    string line, textkey;
    uint value = 0;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> textkey >> value;
      if (textkey == "MemTotal:")
        mem_total = value;
      else if (textkey == "MemFree:")
        mem_free = value;
      else if (textkey == "Buffers:")
        buffers = value;
      else if (textkey == "Cached:")
        cached = value;
      else if (textkey == "Shmem:")
        shmem = value;
      else if (textkey == "SReclaimable:")
        sreclaimable = value;
    }
  }
  uint mem_used = mem_total - mem_free;
  uint cachedmem = cached + sreclaimable - shmem;
  uint non_cachebuf_mem = mem_used - (buffers + cachedmem);
  float mem_util = static_cast<float>(non_cachebuf_mem) / mem_total;
  return mem_util;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string uptime;
  if (filestream.is_open()) {
    string line;
    string idle;  // Note: long for uptime doesn't work!
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  // Note: stol(uptime) doesn't convert correctly
  return static_cast<long>(stof(uptime));
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  vector<string> list_stats;
  if (filestream.is_open()) {
    string stat_value;
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> stat_value) {
      list_stats.push_back(stat_value);
    }
  }
  long utime = std::stol(list_stats[13]);
  long stime = std::stol(list_stats[14]);
  long cutime = std::stol(list_stats[15]);
  long cstime = std::stol(list_stats[16]);
  long totaltime = utime + stime + cutime + cstime;
  return totaltime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, keytext, cputime1, cputime2, cputime3, cputime4, cputime5,
      cputime6, cputime7;
  long cpu_totaltime = 0;
  return cpu_totaltime;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
// ref: https://stackoverflow.com/a/16736599
//      user    nice   system  idle      iowait irq   softirq  steal  guest
//      guest_nice
// cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
vector<uint> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  uint user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0,
       softirq = 0;
  uint steal = 0, guest = 0, guest_nice = 0;
  string line, keytext;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> keytext >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }

  uint all_idle = idle + iowait;
  uint non_idle = user + nice + system + irq + softirq + steal;
  uint total = all_idle + non_idle;
  vector<uint> result = {total, all_idle};
  return result;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, keytext;
  int value = 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keytext >> value;
      if (keytext == "processes") {
        break;
      }
    }
  }
  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, keytext;
  int value = 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keytext >> value;
      if (keytext == "procs_running") {
        break;
      } else
        value = 0.;
    }
  }
  return value;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  string line, cmdline;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cmdline;
  }
  AddOffsetSpace(cmdline, 150);
  return cmdline;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);

  uint value = 0;
  if (filestream.is_open()) {
    string line, keytext;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keytext >> value;
      if (keytext == "VmSize:") {
        value = value / 1024;
        break;
      }
    }
  }
  string result = std::to_string(value);
  AddOffsetSpace(result, 6);
  return result;
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  string line;
  string keytext;
  string value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keytext >> value;
      if (keytext == "Uid:") {
        break;
      }
    }
  }
  return value;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  string str_uid = Uid(pid);
  string line;
  string user;
  string x;
  string uid;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == str_uid) {
        AddOffsetSpace(user, 4);

        break;
      } else {
        user = "";
      }
    }
  }
  return user;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  string stat_value;
  string line;
  vector<string> list_stats;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> stat_value) {
      list_stats.push_back(stat_value);
    }
  }
  float starttime_secs = stof(list_stats[21]) / sysconf(_SC_CLK_TCK);
  long uptime = UpTime() - starttime_secs;
  return uptime;
}

// Add space to avoid text overlapping from the previous entries
void LinuxParser::AddOffsetSpace(std::string& text, int n) {
  int diffspace = n - text.size();
  if (diffspace > 0)
    text.append(diffspace, ' ');  // insert more space
  else if (diffspace < 0)
    text = text.substr(0, n);  // return the substring for 4 bytes length
}