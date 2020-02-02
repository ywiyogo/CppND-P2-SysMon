#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
// Ref:
// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
// Total used memory = MemTotal - MemFree
// Non cache/buffer memory (green) = Total used memory - (Buffers + Cached
// memory) Buffers (blue) = Buffers Cached memory (yellow) = Cached +
// SReclaimable - Shmem Swap = SwapTotal - SwapFree
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line, textkey;
  uint64_t value = 0, mem_total = 0, mem_free = 0, mem_used = 0, buffers = 0;
  uint64_t cached = 0;
  uint64_t cachedmem = 0, non_cachebuf_mem = 0, shmem = 0, sreclaimable = 0;

  if (filestream.is_open()) {
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
  mem_used = mem_total - mem_free;
  cachedmem = cached + sreclaimable - shmem;
  non_cachebuf_mem = mem_used - (buffers + cachedmem);
  float mem_util = static_cast<float>(non_cachebuf_mem) / mem_total;
  // std::cout << "\n mem used: " << mem_used<<"\n"<<"\nMem util:
  // "<<mem_util<<std::flush;
  return mem_util;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long uptime = 0;
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  // std::ifstream filestream(kProcDirectory + kUptimeFilename);
  // long uptime = 0;
  // string line;
  // if (filestream.is_open()) {
  //   std::getline(filestream, line);
  //   std::istringstream linestream(line);
  //   linestream >> uptime;
  // }
  return 0;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
// ref: https://stackoverflow.com/a/16736599
//      user    nice   system  idle      iowait irq   softirq  steal  guest
//      guest_nice
// cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
vector<uint32_t> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  uint32_t user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0,
           softirq = 0;
  uint32_t steal = 0, guest = 0, guest_nice = 0;
  string line, keytext;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> keytext >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }

  uint32_t all_idle = idle + iowait;
  uint32_t non_idle = user + nice + system + irq + softirq + steal;
  uint32_t total = all_idle + non_idle;
  vector<uint32_t> result = {total, all_idle};
  return result;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }