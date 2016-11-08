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

NNPNumberRange *NNPNumberRangeList::getNNPNumberRange(long long int num, stringstream *trace) {
    if (num > 0 && this->data.size() > 0) {

        NNPNumberRange *item = nullptr;
        searchNumberRanges(item, num, avlRoot);
        if (item) {
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
    }
    if (trace != nullptr) {
        *trace << "NOT FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
        *trace << "\n";
    }
    return nullptr;
}

size_t NNPNumberRangeList::dataSize() {
    size_t size = sizeof(NNPNumberRange) * this->data.size() + avlTree.size() * sizeof(NNPNumberRangeTreeNode);

    return size;
}


int NNPNumberRangeList::searchNumberRanges(NNPNumberRange *&numberRange, PhoneNumber num, int64_t p) {
    if (avlRoot == -1) return 0;

    int intersects = 0;
    if (num==avlTree[p].getKey()) {
        for (auto it = avlTree[p].getBorders().begin(); it != avlTree[p].getBorders().end(); ++it) {
            if (numberRange) {
                if (numberRange->getLength() >= (*it).second->getLength())
                    numberRange = (*it).second;
            }
            else
                numberRange = (*it).second;

        }
        intersects += avlTree[p].getBorders().size();
        return intersects;
    }
    for (auto it = avlTree[p].getBorders().begin(); it != avlTree[p].getBorders().end(); ++it) {
        if (num <= (*it).first.second && num >= (*it).first.first) {
            intersects++;
            if (numberRange) {
                if (numberRange->getLength() >= (*it).second->getLength())
                    numberRange = (*it).second;
            }
            else
                numberRange = (*it).second;
        }
    }
    if (num <= avlTree[p].getKey()) {
        if (avlTree[p].getLeftNode() != -1)
            intersects += searchNumberRanges(numberRange, num, avlTree[p].getLeftNode());
    }
    else if (num > avlTree[p].getKey())
        if (avlTree[p].getRightNode() != -1)
            intersects += searchNumberRanges(numberRange, num, avlTree[p].getRightNode());
    return intersects;
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
    avlTree[q].setRightNode(n_node);

    for (auto i = avlTree[n_node].getBorders().begin(); i != avlTree[n_node].getBorders().end(); ) {
        if ((*i).first.first <= avlTree[q].getKey() && (*i).first.second >= avlTree[q].getKey()) {
            NNPNumberRange* upLift = (*i).second;
            avlTree[q].addNNPNumberRange(upLift);
            i = avlTree[n_node].getBorders().erase (i);
            continue;
        }
        i++;
    }

    fixHeight(n_node);
    fixHeight(q);
    return q;
}

int64_t NNPNumberRangeList::rotateLeft(int64_t n_node) // левый поворот вокруг узла
{
    int64_t q = avlTree[n_node].getRightNode();
    avlTree[n_node].setRightNode(avlTree[q].getLeftNode());
    avlTree[q].setLeftNode(n_node);

    for (auto i = avlTree[n_node].getBorders().begin(); i != avlTree[n_node].getBorders().end(); ) {
        if ((*i).first.first <= avlTree[q].getKey() && (*i).first.second >= avlTree[q].getKey()) {
            NNPNumberRange* upLift = (*i).second;
            avlTree[q].addNNPNumberRange(upLift);
            i = avlTree[n_node].getBorders().erase (i);
            continue;
        }
        i++;
    }

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

    if (avlTree[p].isOverlaps(item)) {
        avlTree[p].addNNPNumberRange(item);
        return balance(p);
    }

    if (item->getMidKey() < avlTree[p].getKey())
        avlTree[p].setLeftNode(insertNode(avlTree[p].getLeftNode(), item));
    else
        avlTree[p].setRightNode(insertNode(avlTree[p].getRightNode(), item));

    return balance(p);
}
