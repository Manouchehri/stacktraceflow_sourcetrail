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
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>

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

    size_t leftSiblings() const {
        if (parent == nullptr) {
            return 0;
        }
        auto myIterator = std::find(parent->children.begin(), parent->children.end(), this);
        assert(myIterator != parent->children.end());
        return myIterator - parent->children.begin();
    }

    void remove() {
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

    size_t getSeqNum() const;

private:
    uint32_t number;
    size_t seqNum;
public:
    std::vector<FuncCall*> children;
private:
    FuncCall *parent;
};

#endif // FUNCCALL_H
