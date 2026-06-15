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