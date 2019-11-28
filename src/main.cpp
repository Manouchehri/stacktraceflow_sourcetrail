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
#include "misc.h"

#include <cstdio>
#include <string>
#include <map>
#include <set>
#include <tuple>
#include <string_view>
#include <libgen.h>
#include <cassert>
#include <unistd.h>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/convenience.hpp>

#include <SourcetrailDBWriter.h>
#include <SourceRange.h>
#include <ReferenceKind.h>

using namespace sourcetrail;
using namespace std;

class SdbWriterProxy {
public:
    SdbWriterProxy(const char *path) {
        writer.open(path);
        writer.beginTransaction();
    }
    ~SdbWriterProxy() {
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
                      , 1 });
        SourceRange location;
        location.fileId = fileId;
        location.startLine = location.endLine = (int)call.getLine();
        location.startColumn = 1;
        location.endColumn = 1;
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

void translate(const char *stacktraceflow_input, const char *sourcetraildb_output) {
try {
    StfToSdbTranslator translator;
    SdbWriterProxy writer(sourcetraildb_output);
    auto add_func =
        [&writer, &translator](StackTraceFlowId number, const Function &func) {
            SourcetrailId sourcetraildbId = writer.addFunction(func);
            translator.add(number, sourcetraildbId);
        };
    auto add_call =
        [&writer, &translator](StackTraceFlowId sourceNumber, StackTraceFlowId targetNumber) {
            writer.addCall(translator[sourceNumber], translator[targetNumber]);
        };
    StackTraceFlowReader reader(move(add_func), move(add_call));
    reader.read(stacktraceflow_input);
} catch(ExtensionError &e) {
    fprintf(stderr, "File %s has unsupported extension. Skipping.\n",
            e.getPath().c_str());
} catch(ParsingError &e) {
    fprintf(stderr, "%s\n", e.what());
    exit(1);
} catch (NumberMismatchError &e) {
    fprintf(stderr, "Record file is malformed. This is a bug. Found return "
            "from function number %u while %u was expected. Stopping "
            "parsing prematurely.", e.found, e.expected);
}
}

void run(char *my_filename, int argc, char *argv[]) {
    assert(argc > 0);
    assert(argv[0]);
    assert(argv[0][0]);

    boost::filesystem::create_directories("stacktraceflow_record");

    pid_t my_pid = getpid();
    boost::filesystem::path my_dir =
        boost::dll::program_location().parent_path();
    string valgrindPath = (my_dir / "bin" / "valgrind").native();
    boost::filesystem::path stacktraceflowPrefix =
        boost::filesystem::path("stacktraceflow_record") / 
        (boost::filesystem::basename(argv[0]) + "." + to_string(my_pid));
    string valgrindCmd = valgrindPath +
        " --tool=callgrind" +
        " --stacktraceflow-prefix=" + stacktraceflowPrefix.native();
    for (; *argv; ++argv) {
        valgrindCmd += " "s + *argv;
    }
    printf("Executing:\n    %s\n\n", valgrindCmd.c_str());
    system(valgrindCmd.c_str());
    printf("Success\n"); // TODO: was it, really?
}

void print_usage() {
    printf(
"Welcome to stacktraceflow. To get started, run:\n"
"\n"
"    stacktraceflow --run PROGRAM [ ARG1 [ ARG2 [ .. ] ] ]\n"
"\n"
"This command will record execution of PROGRAM with the given arguments and\n"
"write the data to the (possibly newly created) stacktraceflow_record\n"
"subdirectory.\n"
"\n"
"For best results, PROGRAM should be debug version of your program. I.e., it\n"
"should contain debug symbols, and ideally also be built without\n"
"optimizations.\n"
    );
}

int main(int argc, char *argv[]) {
    if (argc == 4 && argv[1] == "--translate"sv) {
        translate(argv[2], argv[3]);
    } else if (argc > 2 && argv[1] == "--run"sv) {
        run(argv[0], argc - 2, argv + 2);
    } else {
        print_usage();
    }
    return 0;
}
