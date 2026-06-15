#pragma once
#include <istream>
#include <ostream>
#include <string>

class Pinch {
public:
  struct opts {
    int numLines;
    std::string separator;
    std::string bookend;
    std::string endbookend;
    bool verbose;
    bool isPrintableOnly;
    bool quiet;
  };

  struct PinchResult {
    bool isError;
    std::string error;

    std::string getError() const { return error; }
  };

  static PinchResult pinch(std::istream &input, std::ostream &output,
                           const opts &options);
};