#include "processor.h"

#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

Processor::Processor(IOsParser* const parser)
    : p_parser_(parser), prev_totalcpu_(0), prev_idlecpu_(0) {}

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  float result = 0.;
  if (prev_totalcpu_ == 0) {
    vector<uint32_t> cpu_util = p_parser_->CpuUtilization();
    prev_totalcpu_ = cpu_util[0];
    prev_idlecpu_ = cpu_util[1];
  } else {
    vector<uint32_t> cpu_util = p_parser_->CpuUtilization();
    uint32_t d_total = cpu_util[0] - prev_totalcpu_;
    uint32_t d_idle = cpu_util[1] - prev_idlecpu_;
    result = (d_total - d_idle) / static_cast<float>(d_total);

    prev_totalcpu_ = cpu_util[0];
    prev_idlecpu_ = cpu_util[1];
  }

  return result;
}