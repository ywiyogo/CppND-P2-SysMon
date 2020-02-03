#include "format.h"

#include <sstream> //for ostringstream
#include <cstdio>
#include <string>
using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  uint32_t hh = seconds / 3600;
  uint32_t mm = (seconds % 3600) / 60;
  uint32_t ss = (seconds % 3600) % 60;
//   std::ostringstream str_stream;
//   str_stream << hh <<":"<<mm<<":"<<ss; //note: cannot insert 0 for 1 digit
  char timestring[9];
  sprintf(timestring, "%02d:%02d:%02d", hh,mm,ss);
  return string(timestring);
}