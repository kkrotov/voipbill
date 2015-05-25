#include "ThreadLoader.h"

bool ThreadLoader::prepare() {

    if (!init_load_counters_done) {
        Log::info("Loading counters...");
        if (!this->do_load_counters()) {
            return false;
        }
        init_load_counters_done = true;
    }

    if (!init_load_data_done) {
        Log::info("Loading data...");
        if (!this->do_load_data()) {
            return false;
        }
        init_load_data_done = true;
    }

    return true;
}

void ThreadLoader::run() {

    string event;

    try {
        BDbResult res = db_calls.query("SELECT event from event.queue");
        while (res.next()) {

            event = res.get_s(0);

            db_calls.exec("DELETE from event.queue WHERE event='" + event + "'");


            if (event == "clients") {

                data->client.load(&db_calls);

                shared_ptr<ClientCounter> cc = billingData->clientCounter.get();
                if (cc != 0) cc->reload(&db_calls);

            } else if (event == "airp") {

                data->airp.load(&db_calls);

            } else if (event == "number") {

                data->number.load(&db_calls);

            } else if (event == "outcome") {

                data->outcome.load(&db_calls);

            } else if (event == "prefixlist") {

                data->prefixlist.load(&db_calls);

            } else if (event == "prefixlist_prefix") {

                data->prefixlistPrefix.load(&db_calls);

            } else if (event == "release_reason") {

                data->releaseReason.load(&db_calls);

            } else if (event == "route_case") {

                data->routeCase.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table_route") {

                data->routeTableRoute.load(&db_calls);

            } else if (event == "routing_report_data") {

                //

            } else if (event == "trunk") {

                data->trunk.load(&db_calls);
                data->trunkByName.load(&db_calls);
                data->trunkByAlias.load(&db_calls);

            } else if (event == "trunk_number_preprocessing") {

                data->trunkNumberPreprocessing.load(&db_calls);

            } else if (event == "trunk_priority") {

                data->trunkPriority.load(&db_calls);

            } else if (event == "trunk_rule") {

                data->trunkRule.load(&db_calls);

            } else if (event == "defs") {

                data->pricelistPrice.load(&db_calls);

            } else if (event == "instance_settings") {

                data->instanceSettings.load(&db_calls);

            } else if (event == "network_prefix") {

                data->networkPrefix.load(&db_calls);

            } else if (event == "operator") {

                data->voipOperator.load(&db_calls);

            } else if (event == "pricelist") {

                data->pricelist.load(&db_calls);

            } else if (event == "service_number") {

                data->serviceNumber.load(&db_calls);

            } else if (event == "service_trunk") {

                data->serviceTrunk.load(&db_calls);

            } else if (event == "service_trunk_settings") {

                data->serviceTrunkSettings.load(&db_calls);

            } else if (event == "tariff") {

                data->tariff.load(&db_calls);

            } else if (event == "tariff_change_log") {

                data->tariffChangeLog.load(&db_calls);

            } else if (event == "server") {

                data->server.load(&db_calls);

            }

            boost::this_thread::interruption_point();

        }
    } catch (Exception &e) {

        if (event.length() > 0) {
            try {
                db_calls.exec("INSERT INTO event.queue(event)VALUES('" + event + "')");
            } catch (...) {
            }
            event.clear();
        }

        e.addTrace("Loader::run");
        throw e;
    }

}

bool ThreadLoader::do_load_data(BDb *db, DataContainer *data) {
    bool success = true;

    if (data == 0) data = this->data;
    if (db == 0) db = &db_calls;


    try {
        db_calls.exec("DELETE from event.queue");

        data->loadAll(db);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_data");
        Log::exception(e);
        success = false;
    }

    return success;
}

bool ThreadLoader::do_load_counters(BDb *db, DataBillingContainer *billingData) {
    bool success = true;

    if (billingData == 0) billingData = this->billingData;
    if (db == 0) db = &db_calls;

    try {

        billingData->loadAll(db);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_counters");
        Log::exception(e);
        success = false;
    }

    return success;
}

ThreadLoader::ThreadLoader() {
    id = idName();
    name = "Loader";

    data = DataContainer::instance();
    billingData = DataBillingContainer::instance();
    db_calls.setCS(app().conf.db_calls);

    init_load_counters_done = false;
    init_load_data_done = false;
}
