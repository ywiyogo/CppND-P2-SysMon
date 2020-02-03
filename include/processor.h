#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "i_osparser.h"
#include "stdint.h"
class Processor {
 public:
  Processor(IOsParser* const parser);
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  IOsParser* p_parser_;
  uint32_t prev_totalcpu_;
  uint32_t prev_idlecpu_;
};

#endif