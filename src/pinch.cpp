#include "pinch.h"
#include <cstddef>
#include <deque>
#include <math.h>
#include <string>
#include <vector>

Pinch::PinchResult Pinch::pinch(std::istream &input, std::ostream &output,
                                const opts &opts) {

  if (opts.numLines < 0) {
    return Pinch::PinchResult{.isError = true,
                              .error = "Number of lines cannot be negative."};
  }
  const size_t targetLines = static_cast<size_t>(opts.numLines);
  const size_t lineStart = targetLines / 2;
  const size_t lineEnd = targetLines - lineStart;
  size_t line_count = 0;

  std::vector<std::string> head_vector;
  std::deque<std::string> tail_queue;

  std::string line;
  while (std::getline(input, line)) {
    line_count++;
    if (head_vector.size() < lineStart) {
      head_vector.push_back(line);
    } else {
      tail_queue.push_back(line);
      if (tail_queue.size() > lineEnd)
        tail_queue.pop_front();
    }
  }

  if (line_count < targetLines) {
    // write opts.bookend to output
    if (!opts.quiet)
      output << opts.bookend << std::endl;
    // write head_vector to output
    for (const auto &oline : head_vector) {
      output << oline << std::endl;
    }
    // write tail_queue to output
    for (const auto &oline : tail_queue) {
      output << oline << std::endl;
    }
    // write opts.endbookend to output
    if (!opts.quiet)
      output << opts.endbookend << std::endl;
  } else {
    // write opts.bookend to output
    if (!opts.quiet)
      output << opts.bookend << std::endl;
    // write head_vector to output
    for (const auto &oline : head_vector) {
      output << oline << std::endl;
    }
    // write opts.separator to output
    output << opts.separator << std::endl;
    // write tail_queue to output
    for (const auto &oline : tail_queue) {
      output << oline << std::endl;
    }
    // write opts.endbookend to output
    if (!opts.quiet)
      output << opts.endbookend << std::endl;
  }

  if (input.bad()) {
    return Pinch::PinchResult{.isError = true, .error = "Error occurred while reading from input stream."};
  }
  if (!output) {
    return Pinch::PinchResult{.isError = true, .error = "Error occurred while writing to output stream."};
  }

  return Pinch::PinchResult{.isError = false, .error = ""};
}
