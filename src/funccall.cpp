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
#include "funccall.h"
#include "functiondirectory.h"

std::string FuncCall::getName() const
{
    return FunctionDirectory::get().getEntry(number).getName();
}

std::string FuncCall::getPath() const
{
    return FunctionDirectory::get().getEntry(number).getDeclFile();
}

size_t FuncCall::getLine() const
{
    return FunctionDirectory::get().getEntry(number).getLineNum();
}

FuncCall *FuncCall::getParent() const
{
    return parent;
}

void FuncCall::setParent(FuncCall *value)
{
    parent = value;
}

size_t FuncCall::getSeqNum() const
{
    return seqNum;
}
