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
#ifndef FUNCTIONDIRECTORY_H
#define FUNCTIONDIRECTORY_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>

class FuncCall;

class FunctionDirectory
{
public:
    static FunctionDirectory& get() {
        if (instance == nullptr) {
            instance = new FunctionDirectory();
        }
        return *instance;
    }

    void addEntry(uint32_t number, const std::string &funcName, const std::string &path,
                  uint32_t lineNum) {
        if (functions.size() <= number) {
            functions.resize(number + 1);
        }
        functions[number] = {funcName, path, lineNum};
    }

    FuncCall *addFunctionCall(uint32_t number);

    class Entry {
    public:
        Entry() = default;
        Entry(Entry&&) = default;
        Entry(const std::string &_name, const std::string &_declFile, uint32_t lineNum)
            : name(_name), declFile(_declFile), lineNum(lineNum)
        {}
        Entry& operator=(Entry&& other) {
            if (&other == this) {
                return *this;
            }
            assert(other.isValid());
            if (isValid()) {
                if (name == other.name &&
                        declFile == other.declFile &&
                        lineNum == other.lineNum
                ) {
                    std::cerr << "Overwriting Entry with an identical one. "
                              << "Probably duplicate in the dir file." << std::endl;
                } else {
                    std::cerr << "Overwriting Entry with an DIFFERENT one. "
                              << "This is a bug." << std::endl;
                }
                assert(treeNodes.empty());
                treeNodes = std::move(other.treeNodes);
                return *this;
            }
            std::swap(name, other.name);
            std::swap(declFile, other.declFile);
            lineNum = other.lineNum;
            assert(treeNodes.empty());
            std::swap(treeNodes, other.treeNodes);
            return *this;
        }
        Entry(const Entry&) = delete;

        std::string toString() const {
            return name + " " + declFile + ":" + std::to_string(lineNum);
        }

        bool matches(const std::string &pattern) const {
            return this->toString().find(pattern) != std::string::npos;
        }

        bool isValid() const {
            return !name.empty() || !declFile.empty() || lineNum != 0;
        }

        std::string getName() const;

        std::string getDeclFile() const;

        uint32_t getLineNum() const;

        void removeCalls();

    private:
        struct UniquePtrGet {
            FuncCall* operator()(const std::unique_ptr<FuncCall> &ptr) const {
                return ptr.get();
            }
        };
        struct FuncCallIsActive {
            bool operator()(FuncCall *call) const;
        };

    public:
        using CallsIterator = boost::transform_iterator<
            UniquePtrGet,
            std::vector<std::unique_ptr<FuncCall>>::const_iterator>;
        CallsIterator callsBegin() {
            return boost::make_transform_iterator(treeNodes.begin(), UniquePtrGet());
        }
        CallsIterator callsEnd() {
            return boost::make_transform_iterator(treeNodes.end(), UniquePtrGet());
        }

        using ActiveCallsIterator = boost::filter_iterator<FuncCallIsActive, CallsIterator>;
        ActiveCallsIterator activeCallsBegin() {
            return boost::make_filter_iterator(FuncCallIsActive(), callsBegin(), callsEnd());
        }
        ActiveCallsIterator activeCallsEnd() {
            return boost::make_filter_iterator(FuncCallIsActive(), callsEnd(), callsEnd());
        }
    private:
        friend class FunctionDirectory;
        std::string name;
        std::string declFile;
        uint32_t lineNum = 0;
        std::vector<std::unique_ptr<FuncCall>> treeNodes;
    };
    using iterator = std::vector<Entry>::iterator;

private:
    friend class FuncCall;
    FunctionDirectory() {}
    const Entry& getEntry(uint32_t number) const {
        assert(number < functions.size());
        const Entry& result = functions[number];
        assert(result.isValid());
        return result;
    }

    static FunctionDirectory *instance;
    std::vector<Entry> functions;
    size_t totalFunctionCalls = 0;

public:

    iterator begin() {
        return functions.begin();
    }
    iterator end() {
        return functions.end();
    }
};

#endif // FUNCTIONDIRECTORY_H
