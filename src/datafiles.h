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

#include <string>
#include <exception>

#include "funccall.h"

class ParsingError: public std::exception {
public:
    ParsingError(const std::string &_msg) : msg(_msg) {}
    const char *what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
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


FuncCall readRecordFile(const std::string& path);

#endif // DATAFILES_H
