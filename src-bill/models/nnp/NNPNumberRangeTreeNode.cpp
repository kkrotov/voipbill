#include "NNPNumberRangeTreeNode.h"
#include "../../classes/BDb.h"

NNPNumberRangeTreeNode::NNPNumberRangeTreeNode(int64_t index, PhoneNumber from, PhoneNumber to) {

    key = (to + from)/2;
    if (isOverlaps(from, to)) addNNPNumberRange(index, from, to);

    leftNode = -1;
    rightNode = -1;
    height = 1;
}

bool NNPNumberRangeTreeNode::isOverlaps(PhoneNumber from, PhoneNumber to) {
    if ((from <= key) && (key <= to))
        return true;
    else
        return false;
}

void NNPNumberRangeTreeNode::addNNPNumberRange(int64_t index, PhoneNumber from, PhoneNumber to) {
    borders.push_back(make_pair (make_pair(from, to), index));
}

size_t NNPNumberRangeTreeNode::dataSize() {
    return sizeof (NNPNumberRangeTreeNode) + sizeof(Borders) * borders.size();
}
