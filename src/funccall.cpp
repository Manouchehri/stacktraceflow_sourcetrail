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

#include <cassert>

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

void FuncCall::remove() {
    assert(parent != nullptr);
    auto myIterator = std::find(parent->children.begin(), parent->children.end(), this);
    assert(myIterator != parent->children.end());
    auto nextSibling = parent->children.erase(myIterator);
    for (FuncCall *child: children) {
        child->parent = parent;
    }
    parent->children.insert(nextSibling, children.begin(), children.end());
    parent = nullptr;
    children.clear();
}

size_t FuncCall::leftSiblingsCount() const {
    if (parent == nullptr) {
        return 0;
    }
    auto myIterator = std::find(parent->children.begin(), parent->children.end(), this);
    assert(myIterator != parent->children.end());
    return myIterator - parent->children.begin();
}
