#include "RoutingReportDataList.h"

size_t RoutingReportDataList::item_size() {
    return sizeof (RoutingReportData);
}

string RoutingReportDataList::sql(BDb * db) {
    return "   select prefix, routes " \
            "   from routing_report_data " \
            "   where locked = false " \
            "   order by prefix ";
}

inline void RoutingReportDataList::parse_item(BDbResult &row, void * obj) {
    pRoutingReportData item = (pRoutingReportData) obj;
    row.fill_cs(0, item->prefix, sizeof (item->prefix));
    row.fill_cs(1, item->routes, sizeof (item->routes));
}

inline char * RoutingReportDataList::key(const void *obj) {
    return ((pRoutingReportData) obj)->prefix;
}

pRoutingReportData RoutingReportDataList::find(const char * prefix) {
    return (pRoutingReportData) _find(prefix);
}


