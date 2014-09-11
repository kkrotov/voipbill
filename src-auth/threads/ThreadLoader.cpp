#include "ThreadLoader.h"
#include "../classes/AppAuth.h"
#include "../dao/ConfigVersionDao.h"

void ThreadLoader::run() {


    bool needLoadConfig = true;

    auto currentConfig = app().getConfigVersionData();

    if (currentConfig != 0) {
        ConfigVersionDao versionDao(&db_local);

        ConfigVersion * version = versionDao.getCurrentVersion();

        if (version != 0 && version->id == currentConfig->version->id) {
            needLoadConfig = false;
        }

        if (version != 0) delete version;
    }

    if (needLoadConfig) {
        auto data = ConfigVersionData::load(&db_local);
        if (data != 0) {
            app().setConfigVersionData(data);
        } else {
            Log::error("ThreadLoader: Can not load config");
            errors++;
        }
    }
}

void ThreadLoader::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    auto configVersionData = app().getConfigVersionData();

    ObjList * ol;

    if (configVersionData != 0) {
        html << "Version id: <b>" << configVersionData->version->id << "</b><br/>\n";
        html << "Version name: <b>" << configVersionData->version->name << "</b><br/>\n";

        html << "<br/>\n";

        ol = configVersionData->airpList;
        if (ol != 0) html << "Airp: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->numberList;
        if (ol != 0) html << "Number: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->outcomeList;
        if (ol != 0) html << "Outcome: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->prefixlistList;
        if (ol != 0) html << "Prefixlist: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->prefixlistPrefixList;
        if (ol != 0) html << "PrefixlistPrefix: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->routeCaseList;
        if (ol != 0) html << "RouteCase: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->releaseReasonList;
        if (ol != 0) html << "ReleaseReason: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->routeTableList;
        if (ol != 0) html << "RouteTable: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->routeTableRouteList;
        if (ol != 0) html << "RouteTableRoute: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->operatorList;
        if (ol != 0) html << "Operator: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->routingReportDataList;
        if (ol != 0) html << "RoutingReportData: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->trunkList;
        if (ol != 0) html << "Trunk: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = configVersionData->trunkGroupList;
        if (ol != 0) html << "TrunkGroup: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";
    }

    html << "<br/>\n";

    html << "Loader errors count: <b>" << errors << "</b><br/>\n";


}

ThreadLoader::ThreadLoader() {
    id = "loader";
    name = "Loader";

    db_local.setCS(app().conf.db_local);

    errors = 0;
}
