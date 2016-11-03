#pragma once

#include "NNPNumberRange.h"

class NNPNumberRangeTreeNode {
private:

    map<int, NNPNumberRange> ranges;
    int64_t leftNode, rightNode;
    PhoneNumber key;      // ключ узла
    int height; // высота поддерева с корнем в данном узле

public:

    NNPNumberRangeTreeNode(NNPNumberRange *item);

    PhoneNumber getKey() { return key; }

    bool isOverlaps(NNPNumberRange *item);

    void addNNPNumberRange(NNPNumberRange *item);

    int getHeight() { return height; }

    void setHeight(int h) { height = h; }

    void setLeftNode(int64_t node) { leftNode = node; }

    void setRightNode(int64_t node) { rightNode = node; }

    int64_t getLeftNode() { return leftNode; }

    int64_t getRightNode() { return rightNode; }

    unsigned long getIntervalOverlapsCount() { return ranges.size(); }

    size_t dataSize();
};