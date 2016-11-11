#pragma once

#include "NNPNumberRange.h"

class NNPNumberRangeTreeNode {
private:



    int64_t leftNode, rightNode;
    PhoneNumber key;      // ключ узла
    int height; // высота поддерева с корнем в данном узле

    /* Список пар ( (левая граница, правая граница) : NumberRange* ) */
    typedef list<pair<pair<PhoneNumber, PhoneNumber> , int64_t>> Borders;
    Borders borders;

public:


    NNPNumberRangeTreeNode(int64_t index, PhoneNumber from, PhoneNumber to);

    PhoneNumber getKey() { return key; }

    bool isOverlaps(PhoneNumber from, PhoneNumber to);

    void addNNPNumberRange(int64_t index, PhoneNumber from, PhoneNumber to);

    Borders& getBorders () { return borders;}

    int getHeight() { return height; }

    void setHeight(int h) { height = h; }

    void setLeftNode(int64_t node) { leftNode = node; }

    void setRightNode(int64_t node) { rightNode = node; }

    int64_t getLeftNode() { return leftNode; }

    int64_t getRightNode() { return rightNode; }

    unsigned long getIntervalOverlapsCount() { return borders.size(); }

    size_t dataSize();

};
