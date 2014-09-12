#include "RouteTableRouteList.h"

size_t RouteTableRouteList::item_size() {
    return sizeof (RouteTableRoute);
}

string RouteTableRouteList::sql(BDb * db) {
    return "   select  route_table_id, \"order\", " \
            "       a_number_id, b_number_id, " \
            "       outcome_id, outcome_route_table_id " \
            "   from auth.route_table_route " \
            "   order by route_table_id, \"order\" ";
}

inline void RouteTableRouteList::parse_item(BDbResult &row, void * obj) {
    pRouteTableRoute item = (pRouteTableRoute) obj;
    item->route_table_id = row.get_i(0);
    item->order = row.get_i(1);
    item->a_number_id = row.get_i(2);
    item->b_number_id = row.get_i(3);
    item->outcome_id = row.get_i(4);
    item->outcome_route_table_id = row.get_i(5);
    item->key = (item->route_table_id << 10) + item->order;
}

inline int RouteTableRouteList::key(const void *obj) {
    return ((pRouteTableRoute) obj)->key;
}

pRouteTableRoute RouteTableRouteList::find(const int route_table_id, const int order) {
    int key = (route_table_id << 10) + order;
    return (pRouteTableRoute) _find(key);
}


