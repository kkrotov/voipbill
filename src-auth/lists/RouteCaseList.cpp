#include "RouteCaseList.h"

size_t RouteCaseList::item_size() {
    return sizeof (RouteCase);
}

string RouteCaseList::sql(BDb * db) {
    return "   select  id, name " \
            "   from auth.route_case " \
            "   order by id ";
}

inline void RouteCaseList::parse_item(BDbResult &row, void * obj) {
    pRouteCase item = (pRouteCase) obj;
    item->id = row.get_i(0);
    row.fill_cs(1, item->name, sizeof (item->name));
}

inline int RouteCaseList::key(const void *obj) {
    return ((pRouteCase) obj)->id;
}

pRouteCase RouteCaseList::find(const int id) {
    return (pRouteCase) _find(id);
}


