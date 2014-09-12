#pragma once

#include <vector>

#include <boost/algorithm/string.hpp>

using boost::algorithm::split;
using boost::algorithm::is_any_of;

using namespace std;

struct Trunk {
    int id;
    char name[51];
    int number;
    int route_table_id;
};

typedef Trunk * pTrunk;