#include "NNPNumberRangeList.h"

string NNPNumberRangeList::sql(BDb *db) {
    return "select id, country_prefix, ndc, is_mob, " \
            "   is_active, operator_id, region_id, extract(epoch from insert_time), extract(epoch from update_time), city_id, " \
            "   full_number_from, full_number_to " \
            "   from nnp.number_range  " \
            "   where is_active and ( full_number_from <= full_number_to ) and  ( full_number_from >= 0 ) and " \
            "   length(full_number_from::varchar) = length(full_number_to::varchar)  " \
            "   order by full_number_from, full_number_to";
}

void NNPNumberRangeList::parse_item(BDbResult &row, NNPNumberRange *item) {
    item->id = row.get_i(0);
    item->country_prefix = row.get_i(1);
    item->ndc = row.get_i(2);
    item->is_mob = row.get_b(3);
    item->is_active = row.get_b(4);
    item->nnp_operator_id = row.get_i(5);
    item->nnp_region_id = row.get_i(6);
    item->insert_time = row.get_i(7);
    item->update_time = row.get_i(8);
    item->nnp_city_id = row.get_i(9);
    item->full_number_from = row.get_ll(10);
    item->full_number_to = row.get_ll(11);

    avlRoot = insertNode(avlRoot, item);
}

struct NNPNumberRangeList::key_full_number_from {
    bool operator()(const NNPNumberRange &left, long long int num) {
        return left.full_number_from <= num;
    }

    bool operator()(long long int num, const NNPNumberRange &right) {
        return num <= right.full_number_to;
    }
};

NNPNumberRange *NNPNumberRangeList::getNNPNumberRange(long long int num, stringstream *trace) {
    if (num > 0 && this->data.size() > 0) {

        auto begin_orig = this->data.begin();
        auto end_orig = this->data.end();

        auto begin = begin_orig;
        auto end = end_orig;
        {
            int i = 0;
            do {
                auto p = equal_range(begin, end, num, key_full_number_from());
                begin = p.first;
                end = p.second;

                if ((begin != begin_orig) && (begin != end_orig)) {

                    auto item = begin - 1;

                    if (item->full_number_from <= num && num <= item->full_number_to) {
                        if (trace != nullptr) {
                            *trace << "FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
                            *trace << "||";
                            item->dump(*trace);
                            *trace << "\n";
                        }
                        return &*item;
                    }
                }
                i++;
            } while (begin != end && i < 5);
        }
    }
    if (trace != nullptr) {
        *trace << "NOT FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
        *trace << "\n";
    }
    return nullptr;
}

size_t NNPNumberRangeList::dataSize() {

    map<int, int> count;

    size_t size = sizeof(NNPNumberRange) * this->data.size() + avlTree.size() * sizeof(NNPNumberRangeTreeNode);

    for (auto i = avlTree.begin(); i != avlTree.end(); i++) {
        count[i->getIntervalOverlapsCount()] += 1;
        size += i->dataSize();
    }
    return size;
}


void NNPNumberRangeList::searchNumberRanges(set<NNPNumberRange> &numberRangeSet, PhoneNumber num) {
    if (avlRoot == -1) return;


/*
    if (node->getLeftNode() != nullptr) {
        searchNumberRanges(numberRangeSet, num, node->getLeftNode());
    }

    if (node->getRightNode() != nullptr) {
        searchNumberRanges(numberRangeSet, num, node->getRightNode());
    }
*/

}


int NNPNumberRangeList::getBalanceFactor(int64_t n_node)  // возвращает баланс-фактор данного узла.
{
    int leftHeight = 0, rightHeight = 0;
    if (avlTree[n_node].getLeftNode() != -1) leftHeight = avlTree[avlTree[n_node].getLeftNode()].getHeight();
    if (avlTree[n_node].getRightNode() != -1) rightHeight = avlTree[avlTree[n_node].getRightNode()].getHeight();
    return rightHeight - leftHeight;
}

void NNPNumberRangeList::fixHeight(int64_t n_node) { // восстанавливает корректное значение поля height узла
    int leftHeight = 0, rightHeight = 0;
    if (avlTree[n_node].getLeftNode() != -1) leftHeight = avlTree[avlTree[n_node].getLeftNode()].getHeight();
    if (avlTree[n_node].getRightNode() != -1) rightHeight = avlTree[avlTree[n_node].getRightNode()].getHeight();
    avlTree[n_node].setHeight((leftHeight > rightHeight ? leftHeight : rightHeight) + 1);
}

int64_t NNPNumberRangeList::rotateRight(int64_t n_node) // правый поворот вокруг узла
{
    int64_t q = avlTree[n_node].getLeftNode();
    avlTree[n_node].setLeftNode(avlTree[q].getRightNode());
    avlTree[n_node].setRightNode(n_node);
    fixHeight(n_node);
    fixHeight(q);
    return q;
}

int64_t NNPNumberRangeList::rotateLeft(int64_t n_node) // левый поворот вокруг узла
{
    int64_t q = avlTree[n_node].getRightNode();
    avlTree[n_node].setRightNode(avlTree[q].getLeftNode());
    avlTree[n_node].setLeftNode(n_node);
    fixHeight(n_node);
    fixHeight(q);
    return q;
}

int64_t NNPNumberRangeList::balance(int64_t n_node) // балансировка узла
{
    fixHeight(n_node);
    int bf = getBalanceFactor(n_node);
    if (bf == 2) {
        if (getBalanceFactor(avlTree[n_node].getRightNode()) < 0)
            avlTree[n_node].setRightNode(rotateRight(avlTree[n_node].getRightNode()));
        return rotateLeft(n_node);
    }
    if (bf == -2) {
        if (getBalanceFactor(avlTree[n_node].getLeftNode()) > 0)
            avlTree[n_node].setLeftNode(rotateLeft(avlTree[n_node].getLeftNode()));
        return rotateRight(n_node);
    }
    return n_node; // балансировка не нужна
}

int64_t NNPNumberRangeList::insertNode(int64_t p, NNPNumberRange *item) // вставка ключа k в дерево с корнем p
{
    if (item == nullptr) return -1;

    if (p == -1) {
        avlTree.push_back(NNPNumberRangeTreeNode(item));
        return avlTree.size() - 1;
    }

    if (avlTree[p].isOverlaps(item))
        avlTree[p].addNNPNumberRange(item);

    if (item->getMidKey() < avlTree[p].getKey())
        avlTree[p].setLeftNode(insertNode(avlTree[p].getLeftNode(), item));
    else
        avlTree[p].setRightNode(insertNode(avlTree[p].getRightNode(), item));

    return balance(p);
}
