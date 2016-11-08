#pragma once

#include "NNPNumberRange.h"

class NNPNumberRangeTreeNode {
private:



    int64_t leftNode, rightNode;
    PhoneNumber key;      // ключ узла
    int height; // высота поддерева с корнем в данном узле

    /* Список пар ( (левая граница, правая граница) : NumberRange* ) */
    typedef list<pair<pair<PhoneNumber, PhoneNumber> , NNPNumberRange*>> Borders;
    Borders borders;

public:


    NNPNumberRangeTreeNode(NNPNumberRange *item);

    PhoneNumber getKey() { return key; }

    bool isOverlaps(NNPNumberRange *item);

    void addNNPNumberRange(NNPNumberRange *item);

    Borders& getBorders () { return borders;}

    int getHeight() { return height; }  // Возвращает высоту узла

    void setHeight(int h) { height = h; }  // Возвращает высоту узла

    void setLeftNode(int64_t node) { leftNode = node; }

    void setRightNode(int64_t node) { rightNode = node; }

    int64_t getLeftNode() { return leftNode; }

    int64_t getRightNode() { return rightNode; }

    unsigned long getIntervalOverlapsCount() { return borders.size(); }

    size_t dataSize();

};