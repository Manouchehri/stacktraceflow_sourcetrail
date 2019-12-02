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
#include "SdbWriterProxy.h"
#include "Function.h"

int SdbWriterProxy::getFileId(const std::string &filename) {
    auto it = fileToId.find(filename);
    if (it != fileToId.end()) {
        return it->second;
    }
    int result = writer.recordFile(filename);
    fileToId.emplace(filename, result);
    return result;
}

int SdbWriterProxy::write_function(const Function& func) {
    int symbolId = writer.recordSymbol({"::", { {"", func.get_name(), ""} } });
    int fileId = getFileId(func.get_def_file_path());
    writer.recordSymbolScopeLocation(
        symbolId, { fileId
                  , (int)func.get_line()
                  , 1
                  , (int)func.get_line()
                  , 1 });
    SourceRange location;
    location.fileId = fileId;
    location.startLine = location.endLine = (int)func.get_line();
    location.startColumn = 1;
    location.endColumn = 1;
    writer.recordSymbolLocation(symbolId, location);
    return symbolId;
}

void SdbWriterProxy::write_call(SourcetrailId source, SourcetrailId target) {
    const bool isNew = calls.emplace(source, target).second;
    if (!isNew) {
        return;
    }
    writer.recordReference(source, target, ReferenceKind::CALL);
}
