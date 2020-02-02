#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "stdint.h"

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  uint32_t prev_totalcpu_;
  uint32_t prev_idlecpu_;
};

#endif