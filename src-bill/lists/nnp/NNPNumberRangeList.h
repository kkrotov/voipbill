#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPNumberRange.h"
#include "../../classes/AppBill.h"
#include "../../models/nnp/NNPNumberRangeTreeNode.h"

class NNPNumberRangeList : public ObjList<NNPNumberRange> {

protected:

    vector<NNPNumberRangeTreeNode> avlTree;
    int64_t avlRoot = -1;

    string sql(BDb *db);

    inline void parse_item(BDbResult &row, NNPNumberRange *item);


    int64_t insertNode(int64_t p, NNPNumberRange *item); // вставка ключа k в дерево с корнем p

    int getBalanceFactor(int64_t n_node);  // возвращает баланс-фактор данного узла.
    void fixHeight(int64_t n_node);

    int64_t rotateRight(int64_t n_node); // правый поворот вокруг узла
    int64_t rotateLeft(int64_t n_node); // правый поворот вокруг узла

    int64_t balance(int64_t n_node); // балансировка узла

public:

    virtual size_t dataSize();

    NNPNumberRange *getNNPNumberRange(long long int num, stringstream *trace);

    int searchNumberRanges(NNPNumberRange *&numberRange, PhoneNumber num, int64_t p);

};
