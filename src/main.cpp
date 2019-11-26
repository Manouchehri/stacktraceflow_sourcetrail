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
#include "datafiles.h"
#include "functiondirectory.h"

#include <cstdio>
#include <string>
#include <map>
#include <set>
#include <tuple>

#include <SourcetrailDBWriter.h>
#include <SourceRange.h>
#include <ReferenceKind.h>

using namespace sourcetrail;
using namespace std;

class WriterProxy {
public:
    WriterProxy(const char *path) {
        writer.open(path);
        writer.beginTransaction();
    }
    ~WriterProxy() {
        writer.commitTransaction();
        writer.close();
    }

    using FunctionSerialization = tuple<string, string, int>;

    int getFileId(const std::string &filename) {
        auto it = fileToId.find(filename);
        if (it != fileToId.end()) {
            return it->second;
        }
        int result = writer.recordFile(filename);
        fileToId.emplace(filename, result);
        return result;
    }

    static FunctionSerialization functionSerialize(const FunctionDirectory::Entry &function) {
        return {function.getName(), function.getDeclFile(), function.getLineNum()};
    }

    static FunctionSerialization functionSerialize(const FuncCall &call) {
        return {call.getName(), call.getPath(), call.getLine()};
    }

    int getFunctionId(const FuncCall &call) {
        auto functionSer = functionSerialize(call);
        auto it = functionToId.find(functionSer);
        if (it != functionToId.end()) {
            return it->second;
        }
        int symbolId = writer.recordSymbol({"::", { {"", call.getName(), ""} } });
        int fileId = getFileId(call.getPath());
        writer.recordSymbolScopeLocation(
            symbolId, { fileId
                      , (int)call.getLine()
                      , 1
                      , (int)call.getLine()
                      , 999 });
        SourceRange location;
        location.fileId = fileId;
        location.startLine = location.endLine = (int)call.getLine();
        location.startColumn = 1;
        location.endColumn = 999;
        writer.recordSymbolLocation(symbolId, location);
        functionToId.emplace(functionSer, symbolId);
        return symbolId;
    }
    
    void recordCall(const FuncCall &source, const FuncCall &target) {
        const bool isNew =
            calls.emplace(functionSerialize(source), functionSerialize(target)).second;
        if (!isNew) {
            return;
        }
        writer.recordReference(getFunctionId(source), getFunctionId(target),
                               ReferenceKind::CALL);
    }

    void recursivelyRecord(const FuncCall &call) {
        for (const auto &childPtr: call.children) {
            recordCall(call, *childPtr);
            recursivelyRecord(*childPtr);
        }
    }

private:
    SourcetrailDBWriter writer;
    map<string, int> fileToId;
    map<FunctionSerialization, int> functionToId;
    set<pair<FunctionSerialization, FunctionSerialization>> calls;
};

int main(int argc, char *argv[])
{
    try {
        FuncCall functionTree = readRecordFile(argv[1]);
        WriterProxy writer(argv[2]);
        for (auto it = functionTree.children.begin(); it != functionTree.children.end(); ++it) {
            writer.recursivelyRecord(**it);
        }
    } catch(ParsingError &e) {
        fprintf(stderr, "%s\n", e.what());
        exit(1);
    }
}
