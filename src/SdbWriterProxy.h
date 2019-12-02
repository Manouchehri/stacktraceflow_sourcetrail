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
#ifndef SDBWRITERPROXY_H
#define SDBWRITERPROXY_H

#include "misc.h"

#include <map>
#include <set>
#include <string>

#include <SourcetrailDBWriter.h>
#include <SourceRange.h>
#include <ReferenceKind.h>

using namespace sourcetrail;

class Function;

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

    int getFileId(const std::string &filename);
    int write_function(const Function& func);
    void write_call(SourcetrailId source, SourcetrailId target);

private:
    SourcetrailDBWriter writer;
    std::map<std::string, int> fileToId;
    std::set<std::pair<SourcetrailId, SourcetrailId>> calls;
};

#endif // SDBWRITERPROXY_H
