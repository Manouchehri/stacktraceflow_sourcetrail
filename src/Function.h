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
#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>

class Function {
public:
    Function(const std::string& _name, const std::string& _path, uint32_t _line)
        : name(_name)
        , def_file_path(_path)
        , line(_line)
    {}

    const std::string& get_name() const { return name; }
    const std::string& get_def_file_path() const { return def_file_path; }
    uint32_t get_line() const { return line; }

private:
    std::string name;
    std::string def_file_path;
    uint32_t    line;
};

#endif // FUNCTION_H
