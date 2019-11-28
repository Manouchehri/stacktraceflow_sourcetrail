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

#include "StfToSdbTranslator.h"

#include <cassert>

void StfToSdbTranslator::add(StackTraceFlowId stf_id, SourcetrailId sdb_id) {
    if (mapping.size() <= stf_id) {
        mapping.resize(stf_id + 1, SOURCETRAIL_INVALID_ID);
    }
    assert(mapping[stf_id] == SOURCETRAIL_INVALID_ID);
    mapping[stf_id] = sdb_id;
}

SourcetrailId StfToSdbTranslator::operator[](StackTraceFlowId stf_id) const {
    assert(mapping.size() > stf_id);
    assert(mapping[stf_id] != SOURCETRAIL_INVALID_ID);
    return mapping[stf_id];
}
