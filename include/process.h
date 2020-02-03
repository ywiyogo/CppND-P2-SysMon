#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "i_osparser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, IOsParser* const parser);
  int Pid() const;                         // DONE: See src/process.cpp
  std::string User();                      // DONE: See src/process.cpp
  std::string Command();                   // DONE: See src/process.cpp
  float CpuUtilization();                  // DONE: See src/process.cpp
  std::string Ram();                       // DONE: See src/process.cpp
  long int UpTime();                       // DONE: See src/process.cpp
  bool operator<(Process const& a) const;  // DONE: See src/process.cpp
  bool operator>(Process const& a) const;
  bool decending(const Process& a, const Process& b);

  uint* GetPrevCpuTotal();
  ulong* GetPrevTotaltime();
  // DONE: Declare any necessary private members
 private:
  int pid_;
  IOsParser* p_parser_;
  uint ram_;
  uint prev_cputotaltime_;
  ulong prev_totaltime_;
  float cpu_util_;
};

#endif