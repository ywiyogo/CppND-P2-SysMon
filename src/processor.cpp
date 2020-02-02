#include "processor.h"

#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using namespace std;

Processor::Processor() : prev_totalcpu_(0), prev_idlecpu_(0) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<uint32_t> cpu_util;
  float result = 0.;
  if (prev_totalcpu_ == 0) {
    result = 1.;
    cpu_util = LinuxParser::CpuUtilization();
    prev_totalcpu_ = cpu_util[0];
    prev_idlecpu_ = cpu_util[1];
  } else {
    cpu_util = LinuxParser::CpuUtilization();
    uint32_t d_total = cpu_util[0] - prev_totalcpu_;
    uint32_t d_idle = cpu_util[1] - prev_idlecpu_;
    result = (d_total - d_idle) / static_cast<float>(d_total);

    prev_totalcpu_ = cpu_util[0];
    prev_idlecpu_ = cpu_util[1];
  }

  return result;
}