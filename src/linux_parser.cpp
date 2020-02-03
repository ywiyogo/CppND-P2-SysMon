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
  // DIR* directory = opendir(kProcDirectory.c_str());
  // struct dirent* file;
  // while ((file = readdir(directory)) != nullptr) {
  //   // Is this a directory?
  //   if (file->d_type == DT_DIR) {
  //     // Is every character of the name a digit?
  //     string filename(file->d_name);
  //     if (std::all_of(filename.begin(), filename.end(), isdigit)) {
  //       int pid = stoi(filename);
  //       pids.push_back(pid);
  //     }
  //   }
  // }
  // closedir(directory);
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

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line, uptime;  // Note: long for uptime doesn't work!
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  string stat_value, line;
  vector<string> list_stats;
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> stat_value) {
      list_stats.push_back(stat_value);
    }
  }
  utime = std::stol(list_stats[13]);
  stime = std::stol(list_stats[14]);
  cutime = std::stol(list_stats[15]);
  cstime = std::stol(list_stats[16]);
  long totaltime = utime + stime + cutime + cstime;
  return totaltime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, keytext, cputime1, cputime2, cputime3, cputime4, cputime5,
      cputime6, cputime7;
  long cpu_totaltime = 0;

  // if (filestream.is_open()) {
  //   while (std::getline(filestream, line)) {
  //     std::istringstream linestream(line);
  //     linestream >> keytext >> cputime1 >> cputime2 >> cputime3 >> cputime4
  //     >>
  //         cputime5 >> cputime6 >> cputime7;
  //     if (keytext == "cpu") {
  //       cpu_totaltime = stol(cputime1) + stol(cputime2) + stol(cputime3) +
  //       stol(cputime4) + stol(cputime5) +
  //                       stol(cputime6) + stol(cputime7);
  //       break;
  //     }
  //   }
  // }
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
  string line, keytext;
  uint value = 0;
  if (filestream.is_open()) {
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
  string line, keytext, value;
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
  string line, user, x, uid;
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
  string stat_value, line;
  long uptime = 0., starttime_secs = 0;
  vector<string> list_stats;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> stat_value) {
      list_stats.push_back(stat_value);
    }
  }
  starttime_secs = stol(list_stats[21]) / sysconf(_SC_CLK_TCK);
  uptime = UpTime() - starttime_secs;
  if (uptime <= 0) std::cout << "PID: " << pid << std::flush;
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