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
#ifndef MISC_H
#define MISC_H

#include <limits>
#include <cstdint>

using StackTraceFlowId = uint32_t;
using SourcetrailId = int;

constexpr static StackTraceFlowId STACKTRACEFLOW_INVALID_ID =
    std::numeric_limits<StackTraceFlowId>::max();

constexpr static SourcetrailId SOURCETRAIL_INVALID_ID =
    std::numeric_limits<SourcetrailId>::min();

#endif // STACKTRACEFLOWREADER_H
