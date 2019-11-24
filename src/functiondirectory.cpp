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
#include <iostream>

#include "functiondirectory.h"
#include "logic/funccall.h"

using namespace std;

FunctionDirectory* FunctionDirectory::instance = nullptr;

std::string FunctionDirectory::Entry::getName() const
{
    return name;
}

std::string FunctionDirectory::Entry::getDeclFile() const
{
    return declFile;
}

uint32_t FunctionDirectory::Entry::getLineNum() const
{
    return lineNum;
}

void FunctionDirectory::Entry::removeCalls() {
    for (auto& call : treeNodes) {
        call->remove();
    }
}

FuncCall *FunctionDirectory::addFunctionCall(uint32_t number) {
    if (number >= functions.size()) {
        functions.resize(number+1);
        cerr << "Extending FunctionDirectory when it's supposed to be already read in. "
             << "This is a bug." << endl;
    }
    if (!functions[number].isValid()) {
        functions[number] = {"?DUMMY?", "?DUMMY?", 0};
        cerr << "Inserting a dummy Entry into FunctionDirectory when it's supposed to be already read in. "
             << "This is a bug." << endl;
    }
    functions[number].treeNodes.emplace_back(new FuncCall(number, ++totalFunctionCalls));
    return functions[number].treeNodes.back().get();
}

const FunctionDirectory::SearchIterator FunctionDirectory::SearchIterator::end =
    FunctionDirectory::SearchIterator();

bool FunctionDirectory::Entry::FuncCallIsActive::operator()(FuncCall *call) const {
    assert(call != nullptr);
    return call->isActive();
}
