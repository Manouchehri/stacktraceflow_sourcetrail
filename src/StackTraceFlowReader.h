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
#ifndef STACKTRACEFLOWREADER_H
#define STACKTRACEFLOWREADER_H

#include "misc.h"

#include <functional>
#include <exception>
#include <string>
#include <fstream> // TODO: get rid of this

class Function;

class StackTraceFlowReader {
public:
    using AddFunctionCallback = std::function<void(StackTraceFlowId, const Function&)>;
    using AddCallCallback = std::function<void(StackTraceFlowId, StackTraceFlowId)>;

    StackTraceFlowReader(AddFunctionCallback &&_add_function, 
                         AddCallCallback &&_add_call)
        : add_function(std::move(_add_function))
        , add_call(std::move(_add_call))
    {}
    StackTraceFlowReader(const StackTraceFlowReader&) = delete;
    StackTraceFlowReader(StackTraceFlowReader&&) = delete;
    StackTraceFlowReader& operator=(const StackTraceFlowReader&) = delete;
    StackTraceFlowReader& operator=(StackTraceFlowReader&&) = delete;

    void read(const char* input_path);

private:
    static std::string recordPathToDirectoryPath(const std::string& recordPath);
    void readDirectoryFile(const std::string& path);
    void parseFunctionCall(std::ifstream &recordStream, StackTraceFlowId);

    AddFunctionCallback add_function;
    AddCallCallback add_call;
};

class ParsingError: public std::exception {
public:
    ParsingError(const std::string &_msg) : msg(_msg) {}
    const char *what() const throw() {
        return msg.c_str();
    }

private:
    const std::string msg;
};

/* Found a return from an unexpected function in the record file */
class NumberMismatchError: public std::exception {
public:
    NumberMismatchError(uint32_t _expected, uint32_t _found)
        : expected(_expected)
        , found(_found)
    { }

    const uint32_t expected;
    const uint32_t found;
};

class ExtensionError: public std::exception {
public:
    ExtensionError(const std::string &_path)
        : path(_path)
    { }
    const std::string &getPath() const { return path; }

private:
    const std::string path;
};

#endif // STACKTRACEFLOWREADER_H
