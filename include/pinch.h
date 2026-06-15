#pragma once
#include <istream>
#include <ostream>
#include <string>

class Pinch {
public:
  struct opts {
    int numLines;
    bool lineNumbers;
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

  struct TLine {
    size_t linenumber;
    std::string line;

    std::string get(bool showLineNumbers) const {
      if (showLineNumbers) {
        return std::to_string(linenumber) + ": " + line;
      }
      return line;
    }
  };

  static PinchResult pinch(std::istream &input, std::ostream &output,
                           const opts &options);

  static std::string stripNonPrintable(const std::string &input,
                                       bool &hasStripped);
};