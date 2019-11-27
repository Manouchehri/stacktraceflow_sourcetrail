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
#ifndef FUNCCALL_H
#define FUNCCALL_H

#include <vector>
#include <string>

class FuncCall
{
public:
    explicit FuncCall(uint32_t _number, size_t _seqNum)
        : number(_number)
        , seqNum(_seqNum)
        , parent(nullptr)
    { }
    ~FuncCall() {}
    FuncCall(FuncCall&& other)
        : number(other.number)
        , seqNum(other.seqNum)
        , children(std::move(other.children))
        , parent(other.parent)
    {
        for (auto& child: children) {
            child->parent = this;
        }
    }

    FuncCall(const FuncCall&) = delete;
    FuncCall& operator=(const FuncCall&) = delete;
    FuncCall& operator=(FuncCall&&) = delete;

    std::string getName() const;

    std::string getPath() const;

    size_t getLine() const;

    FuncCall *getParent() const;
    void setParent(FuncCall *value);

    bool isActive() const {
        return parent != nullptr;
    }

    size_t childCount() const {
        return children.size();
    }

    size_t leftSiblingsCount() const;

    size_t getSeqNum() const;

    void remove();

    void addChild(FuncCall* child) {
        children.push_back(child);
        children.back()->parent = this;
    }

    FuncCall *child(size_t idx) {
        if (idx < children.size()) {
            return children[idx];
        }
        return nullptr;
    }

private:
    uint32_t number;
    size_t seqNum;
public: //TODO: make private
    std::vector<FuncCall*> children;
private:
    FuncCall *parent;
};

#endif // FUNCCALL_H
