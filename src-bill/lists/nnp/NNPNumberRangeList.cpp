#include "NNPNumberRangeList.h"
#include <algorithm>

string NNPNumberRangeList::sql(BDb *db) {
    return "select id, country_prefix, ndc, is_mob, " \
            "   is_active, operator_id, region_id, extract(epoch from insert_time), extract(epoch from update_time), city_id, " \
            "   full_number_from, full_number_to, country_code, ndc_type_id " \
            "   from nnp.number_range  " \
            "   where is_active and ( full_number_from <= full_number_to ) and  ( full_number_from >= 0 ) and " \
            "   length(full_number_from::varchar) = length(full_number_to::varchar)  " \
            "   order by id";
}

void NNPNumberRangeList::parse_item(BDbResult &row, NNPNumberRange *item) {
    return ;
}

NNPNumberRange *NNPNumberRangeList::getNNPNumberRange(long long int num, stringstream *trace) {
    if (num > 0 && this->data.size() > 0) {

        int64_t result = -1;
        searchNumberRanges(result, num, avlRoot);
        if (result >= 0) {
            if (data[result].full_number_from <= num && num <= data[result].full_number_to) {
                if (trace != nullptr) {
                    *trace << "FOUND|NNPNumberRange|BY NUM '" << num << "'" << "\n";
                    *trace << "||";
                    data[result].dump(*trace);
                    *trace << "\n";
                }
                return &(data[result]);
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
    size_t size = sizeof(NNPNumberRange) * this->data.size();
    for (auto i : avlTree)
        size += i.dataSize();
    return size;
}


int NNPNumberRangeList::searchNumberRanges(int64_t &result, PhoneNumber num, int64_t p) {
    if (avlRoot == -1) return 0;

    int intersects = 0;
    if (num==avlTree[p].getKey()) {
        for (auto it = avlTree[p].getBorders().begin(); it != avlTree[p].getBorders().end(); ++it) {
            if (result >= 0) {
                if (data[result].getLength() >= data[(*it).second].getLength())
                    result = (*it).second;
            }
            else
                result = (*it).second;

        }
        intersects += avlTree[p].getBorders().size();
        return intersects;
    }
    for (auto it = avlTree[p].getBorders().begin(); it != avlTree[p].getBorders().end(); ++it) {
        if (num <= (*it).first.second && num >= (*it).first.first) {
            intersects++;
            if (result >= 0) {
                if (data[result].getLength() >= data[(*it).second].getLength())
                    result = (*it).second;
            }
            else
                result = (*it).second;
        }
    }
    if (num <= avlTree[p].getKey()) {
        if (avlTree[p].getLeftNode() != -1)
            intersects += searchNumberRanges(result, num, avlTree[p].getLeftNode());
    }
    else if (num > avlTree[p].getKey())
        if (avlTree[p].getRightNode() != -1)
            intersects += searchNumberRanges(result, num, avlTree[p].getRightNode());
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
            int64_t upLift = (*i).second;
            avlTree[q].addNNPNumberRange(upLift, data[upLift].full_number_from, data[upLift].full_number_to);
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
            int64_t upLift = (*i).second;
            avlTree[q].addNNPNumberRange(upLift, data[upLift].full_number_from, data[upLift].full_number_to);
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

int64_t NNPNumberRangeList::insertNode(int64_t p, int64_t index) // вставка ключа k в дерево с корнем p
{
    if (p == -1) {
        avlTree.push_back(NNPNumberRangeTreeNode(index, data[index].full_number_from, data[index].full_number_to));
        return avlTree.size() - 1;
    }

    if (avlTree[p].isOverlaps(data[index].full_number_from, data[index].full_number_to)) {
        avlTree[p].addNNPNumberRange(index, data[index].full_number_from, data[index].full_number_to);
        return balance(p);
    }

    if (data[index].getMidKey() < avlTree[p].getKey())
        avlTree[p].setLeftNode(insertNode(avlTree[p].getLeftNode(), index));
    else
        avlTree[p].setRightNode(insertNode(avlTree[p].getRightNode(), index));

    return balance(p);
}
void NNPNumberRangeList::load (BDb *db) {
    if (!db)
        return ;
    BDbResult res = db->query(sql(db));

    loadtime = time(NULL);

    this->data.clear();
    this->data.resize(res.size());

    while (res.next()) {
        parse_item_by_index(res, res.position());
    }
    after_load();
}

void NNPNumberRangeList::parse_item_by_index (BDbResult &row, int64_t index) {
    data[index].id = row.get_i(0);
    data[index].country_prefix = row.get_i(1);
    data[index].ndc = row.get_i(2);
    data[index].is_mob = row.get_b(3);
    data[index].is_active = row.get_b(4);
    data[index].nnp_operator_id = row.get_i(5);
    data[index].nnp_region_id = row.get_i(6);
    data[index].insert_time = row.get_i(7);
    data[index].update_time = row.get_i(8);
    data[index].nnp_city_id = row.get_i(9);
    data[index].full_number_from = row.get_ll(10);
    data[index].full_number_to = row.get_ll(11);
    data[index].country_code = row.get_i(12);
    data[index].ndc_type_id = row.get_i(13);

    avlRoot = insertNode(avlRoot, index);
}
struct NNPNumberRangeList::key_number_range_id {
    bool operator()(const NNPNumberRange &left, int nnp_id) {
        return left.id < nnp_id;
    }

    bool operator()(int nnp_id, const NNPNumberRange &right) {
        return nnp_id < right.id;
    }
};
NNPNumberRange* NNPNumberRangeList::getNNPNumberRangeById(int id) {
    if ( id < 1 || data.size() < 1) return nullptr;
    auto begin = data.begin();
    auto end = data.end();
    auto it = std::equal_range(begin, end, id, key_number_range_id());
    begin = it.first;
    end = it.second;
    if (begin != end){
        return &(*begin);
    }
    return nullptr;
}
