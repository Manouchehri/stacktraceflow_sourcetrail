/*
 Copyright 2019 Wojciech Baranowski <wbaranowski@protonmail.com>

 This file is part of stacktraceflow.

 stacktraceflow is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 stacktraceflow is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with stacktraceflow. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DATAFILES_H
#define DATAFILES_H

#include <tuple>
#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <regex>
#include <endian.h>

#include "funccall.h"
#include "functiondirectory.h"

class ParsingError: public std::exception {
public:
    ParsingError(const std::string &_msg) : msg(_msg) {}
    const char *what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

class NumberMismatchError: public std::exception {
public:
    NumberMismatchError(uint32_t _expected, uint32_t _found)
        : expected(_expected)
        , found(_found)
    { }

    const uint32_t expected;
    const uint32_t found;
};

std::string recordPathToDirectoryPath(const std::string& recordPath) {
    std::string result = recordPath;
    size_t extensionPos = result.find(".stfr.");
    if (extensionPos == std::string::npos) {
        throw ParsingError("Record filename '" + recordPath + "' does not end with " +
                           ".stfr.[thread_number]");
    }
    result.replace(extensionPos, std::string::npos, ".stfd");
    return result;
}

void readDirectoryFile(const std::string& path) {
    std::ifstream dirStream(path);
    std::string line;
    std::regex dirEntryRe("^([[:digit:]]+) (.+) @(.+):([[:digit:]]+)$");
    while (std::getline(dirStream, line)) {
        std::smatch reMatches;
        if (!std::regex_match(line, reMatches, dirEntryRe)) {
            throw ParsingError("Line '" + line + "' in the directory file '" + path + "' could not be parsed");
        }
        assert(reMatches.size() > 4);
        FunctionDirectory::get().addEntry(
            std::stoul(reMatches[1]),
            reMatches[2],
            reMatches[3],
            std::stoul(reMatches[4])
        );
    }
}

void parseFunctionCall(std::ifstream &recordStream, FuncCall &parent) {
    uint32_t funcNumber;
    recordStream.read(reinterpret_cast<char *>(&funcNumber), sizeof(funcNumber));
    funcNumber = le32toh(funcNumber);
    FuncCall *currentCall= FunctionDirectory::get().addFunctionCall(funcNumber);
    parent.addChild(currentCall);

    char sign;
    recordStream.read(&sign, 1);
    while (sign == '+') {
        parseFunctionCall(recordStream, *currentCall);
        recordStream.read(&sign, 1);
    }
    assert(sign == '-');
    uint32_t exitFuncNumber;
    recordStream.read(reinterpret_cast<char *>(&exitFuncNumber), sizeof(exitFuncNumber));
    exitFuncNumber = le32toh(exitFuncNumber);
    if (exitFuncNumber != funcNumber) {
        throw NumberMismatchError(funcNumber, exitFuncNumber);
    }
}

FuncCall readRecordFile(const std::string& path) {
    readDirectoryFile(recordPathToDirectoryPath(path));
    FuncCall result(0, 0);
    std::ifstream recordStream(path, std::ifstream::binary | std::ifstream::in);
    try {
        while (true) {
            char sign;
            recordStream.read(&sign, 1);
            if (recordStream.eof()) {
                break;
            }
            if (sign != '+') {
                throw ParsingError("Expected '+' in record file but found '" +
                                   std::string(1, sign) + "'");
            }
            parseFunctionCall(recordStream, result);
        }
    } catch (NumberMismatchError &e) {
        std::cerr << "Record file is malformed. This is a bug. Found exit from function number "
                  << e.found << " while " << e.expected << " was expected. "
                  << "Stopping parsing prematurely." << std::endl;
    }
    return result;
}

#endif // DATAFILES_H
