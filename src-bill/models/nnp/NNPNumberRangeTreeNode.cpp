#include "NNPNumberRangeTreeNode.h"

NNPNumberRangeTreeNode::NNPNumberRangeTreeNode(NNPNumberRange *item) {

    if (item != nullptr) {
        key = item->getMidKey();
        if (isOverlaps(item)) addNNPNumberRange(item);

    } else key = 0;

    leftNode = -1;
    rightNode = -1;
    height = 1;
}

bool NNPNumberRangeTreeNode::isOverlaps(NNPNumberRange *item) {
    if (item == nullptr) return false;
    if ((item->full_number_from <= key) && (key <= item->full_number_to))
        return true;
    else
        return false;
}

void NNPNumberRangeTreeNode::addNNPNumberRange(NNPNumberRange *item) {
    if (item != nullptr) {
        ranges[item->id] = *item;
    }
}

size_t NNPNumberRangeTreeNode::dataSize() {
    return sizeof(map<int, NNPNumberRange>) * ranges.size();
}

