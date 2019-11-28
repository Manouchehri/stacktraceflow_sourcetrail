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

#include "StackTraceFlowReader.h"

#include <string>

using namespace std;

void StackTraceFlowReader::read(const char* input_path) {
    readDirectoryFile(recordPathToDirectoryPath(input_path));
    FuncCall result(0, 0);
    std::ifstream recordStream(input_path, std::ifstream::binary | std::ifstream::in);
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
    return result;
}

string StackTraceFlowReader::recordPathToDirectoryPath(const string& recordPath) {
    std::string result = recordPath;
    size_t extensionPos = result.find(".stfr.");
    if (extensionPos == std::string::npos) {
        throw ExtensionError(recordPath);
    }
    result.replace(extensionPos, std::string::npos, ".stfd");
    return result;
}

void StackTraceFlowReader::readDirectoryFile(const string& path) {
    std::ifstream dirStream(path);
    std::string line;
    std::regex dirEntryRe("^([[:digit:]]+) (.+) @(.+):([[:digit:]]+)$");
    while (std::getline(dirStream, line)) {
        std::smatch reMatches;
        if (!std::regex_match(line, reMatches, dirEntryRe)) {
            throw ParsingError("Line '" + line + "' in the directory file '" + path + "' could not be parsed");
        }
        assert(reMatches.size() > 4);

        StackTraceFlowId number = std::stoul(reMatches[1]);
        Function entry(reMatches[2], reMatches[3], std::stoul(reMatches[4]));
        add_function(number, entry);
    }
}

void StackTraceFlowReader::parseFunctionCall(std::ifstream &recordStream, FuncCall &parent) {
    static_assert(is_same_v<uint32_t, StackTraceFlowId>);
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
