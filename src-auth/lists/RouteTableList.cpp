#include "RouteTableList.h"

size_t RouteTableList::item_size() {
    return sizeof (RouteTable);
}

string RouteTableList::sql(BDb * db) {
    return "   select  id, name " \
            "   from route_table " \
            "   order by id ";
}

inline void RouteTableList::parse_item(BDbResult &row, void * obj) {
    pRouteTable item = (pRouteTable) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
}

inline int RouteTableList::key(const void *obj) {
    return ((pRouteTable) obj)->id;
}

pRouteTable RouteTableList::find(const int id) {
    return (pRouteTable) _find(id);
}


