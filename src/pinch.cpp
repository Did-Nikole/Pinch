/*
 * Copyright (C) 2026 Nikole Smith (ApptsolutioNZ - appsolutionz.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "pinch.h"
#include <cctype>
#include <cstddef>
#include <deque>
#include <iostream>
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

  std::vector<Pinch::TLine> head_vector;
  std::deque<Pinch::TLine> tail_queue;

  bool warned_about_stripping = false;
  std::string line;
  while (std::getline(input, line)) {
    line_count++;
    if (opts.isPrintableOnly) {
      bool hasStripped = false;
      line = stripNonPrintable(line, hasStripped);
      if (hasStripped && !warned_about_stripping) {
        std::cerr << "Warning: Input contains non-printable characters, escape "
                     "sequences, or binary data. Stripping..."
                  << std::endl;
        warned_about_stripping = true;
      }
    }
    if (head_vector.size() < lineStart) {
      head_vector.push_back({.linenumber = line_count, .line = line});
    } else {
      tail_queue.push_back({.linenumber = line_count, .line = line});
      if (tail_queue.size() > lineEnd)
        tail_queue.pop_front();
    }
  }

  if (line_count < targetLines) {
    // write opts.bookend to output
    if (!opts.quiet)
      output << opts.bookend << std::endl;
    // write head_vector to output
    for (const auto &oline : head_vector)
      output << oline.get(opts.lineNumbers) << std::endl;
    // write tail_queue to output
    for (const auto &oline : tail_queue)
      output << oline.get(opts.lineNumbers) << std::endl;
    // write opts.endbookend to output
    if (!opts.quiet)
      output << opts.endbookend << std::endl;
  } else {
    // write opts.bookend to output
    if (!opts.quiet)
      output << opts.bookend << std::endl;
    // write head_vector to output
    for (const auto &oline : head_vector)
      output << oline.get(opts.lineNumbers) << std::endl;

    // write opts.separator to output
    output << opts.separator << std::endl;
    // write tail_queue to output
    for (const auto &oline : tail_queue)
      output << oline.get(opts.lineNumbers) << std::endl;

    // write opts.endbookend to output
    if (!opts.quiet)
      output << opts.endbookend << std::endl;
  }

  if (input.bad()) {
    return Pinch::PinchResult{
        .isError = true,
        .error = "Error occurred while reading from input stream."};
  }
  if (!output) {
    return Pinch::PinchResult{
        .isError = true,
        .error = "Error occurred while writing to output stream."};
  }

  return Pinch::PinchResult{.isError = false, .error = ""};
}

std::string Pinch::stripNonPrintable(const std::string &input,
                                     bool &hasStripped) {
  std::string cleaned;
  cleaned.reserve(input.size());

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];
    // Check if it starts an ANSI escape sequence: ESC (0x1b)
    if (c == '\x1b') {
      hasStripped = true;
      if (i + 1 < input.size() && input[i + 1] == '[') {
        i += 2;
        while (i < input.size() &&
               !std::isalpha(static_cast<unsigned char>(input[i]))) {
          i++;
        }
        continue;
      }
    }

    if (std::isprint(static_cast<unsigned char>(c)) || c == '\t' || c == '\r' ||
        c == '\n') {
      cleaned.push_back(c);
    } else {
      hasStripped = true;
    }
  }
  return cleaned;
}
