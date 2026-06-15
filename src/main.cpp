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
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#define ARGSPARSERRENEWED_IMPLEMENTATION

#include "ArgsParser.hpp"

int main(int argc, char **argv) {

  std::map<std::string, std::optional<ArgValue>> results;
  ArgsParser parser;
  // create pointers to buffererd io
  std::stringstream ss_input;
  std::stringstream ss_output;

  std::istream *input_ptr = &std::cin;
  std::ostream *output_ptr = &std::cout;

  parser.addArgument({.type = ArgType::BOOLEAN,
                      .shortFlag = "-h",
                      .longFlag = "--help",
                      .description = "Display help message",
                      .required = false,
                      .defaultValue = false});

  parser.addArgument({.type = ArgType::ULONGLONG,
                      .shortFlag = "-n",
                      .longFlag = "--number",
                      .description = "Maximum number of lines to display",
                      .required = false,
                      .defaultValue = 20ULL});

  parser.addArgument({.type = ArgType::STRING,
                      .shortFlag = "-i",
                      .longFlag = "--input",
                      .description = "Input file",
                      .required = false,
                      .defaultValue = std::string("stdin")});

  parser.addArgument({.type = ArgType::STRING,
                      .shortFlag = "-o",
                      .longFlag = "--output",
                      .description = "Output file",
                      .required = false,
                      .defaultValue = std::string("stdout")});

  parser.addArgument({.type = ArgType::STRING,
                      .shortFlag = "-s",
                      .longFlag = "--separator",
                      .description = "Separator text",
                      .required = false,
                      .defaultValue = std::string("[... TRUNCATED ...]")});

  parser.addArgument({.type = ArgType::STRING,
                      .shortFlag = "-b",
                      .longFlag = "--bookend",
                      .description = "Bookend marker",
                      .required = false,
                      .defaultValue = std::string("[ SOF ]")});

  parser.addArgument({.type = ArgType::STRING,
                      .shortFlag = "-e",
                      .longFlag = "--endbookend",
                      .description = "Endbookend marker",
                      .required = false,
                      .defaultValue = std::string("[ EOF ]")});

  parser.addArgument({.type = ArgType::BOOLEAN,
                      .shortFlag = "-q",
                      .longFlag = "--quiet",
                      .description = "Quiet mode. Suppresses the -b and -e"
                                     "markers completely (outputs only data and"
                                     " the -s separator).",
                      .required = false,
                      .defaultValue = false});

  parser.addArgument({.type = ArgType::BOOLEAN,
                      .shortFlag = "-p",
                      .longFlag = "--printable",
                      .description =
                          "Printable only. Strips non-printable control "
                          "characters, escape sequences, and binary data.",
                      .required = false,
                      .defaultValue = false});

  parser.addArgument({.type = ArgType::BOOLEAN,
                      .shortFlag = "-l",
                      .longFlag = "--lines",
                      .description = "Line numbers."
                                     " Adds line numbers to the output.",
                      .required = false,
                      .defaultValue = false});

  if (!parser.parse(argc, argv, results)) {
    parser.showHelp();
    return 1;
  }

  if (results.at("help").has_value() &&
      std::get<bool>(results.at("help").value())) {
    parser.showHelp();
    return 0;
  }

  if (results.at("input").has_value()) {
    auto instring = std::get<std::string>(results.at("input").value());
    if (instring != "stdin") {
      auto *infile = new std::ifstream(instring);
      if (!infile->is_open()) {
        std::cerr << "Error: Cannot open input file '" << instring
                  << "': " << std::strerror(errno) << std::endl;
        delete infile;
        return 1;
      }
      input_ptr = infile;
    }
  }

  if (results.at("output").has_value()) {
    auto outstring = std::get<std::string>(results.at("output").value());
    if (outstring != "stdout") {
      auto *outfile = new std::ofstream(outstring);
      if (!outfile->is_open()) {
        std::cerr << "Error: Cannot open output file '" << outstring
                  << "': " << std::strerror(errno) << std::endl;
        delete outfile;
        if (input_ptr != &std::cin) {
          delete input_ptr;
        }
        return 1;
      }
      output_ptr = outfile;
    }
  }

  Pinch::opts opts;

  if (results.at("number").has_value()) {
    opts.numLines = std::get<unsigned long long>(results.at("number").value());
  }

  if (results.at("separator").has_value()) {
    opts.separator = std::get<std::string>(results.at("separator").value());
  }

  if (results.at("bookend").has_value()) {
    opts.bookend = std::get<std::string>(results.at("bookend").value());
  }

  if (results.at("endbookend").has_value()) {
    opts.endbookend = std::get<std::string>(results.at("endbookend").value());
  }

  if (results.at("printable").has_value()) {
    opts.isPrintableOnly = std::get<bool>(results.at("printable").value());
  }

  if (results.at("quiet").has_value()) {
    opts.quiet = std::get<bool>(results.at("quiet").value());
  }

  if (results.at("lines").has_value()) {
    opts.lineNumbers = std::get<bool>(results.at("lines").value());
  }

  auto pinch_result = Pinch::pinch(*input_ptr, *output_ptr, opts);

  if (input_ptr != &std::cin) {
    delete input_ptr;
  }
  if (output_ptr != &std::cout) {
    delete output_ptr;
  }

  if (pinch_result.isError) {
    std::cerr << pinch_result.getError() << std::endl;
    return 1;
  }

  return 0;
}