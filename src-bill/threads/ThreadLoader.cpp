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

    try {
        BDbResult res = db_calls.query("SELECT event, version from event.queue");
        while (res.next()) {

            string event = res.get_s(0);
            long long int version = res.get_ll(1);
            current_event = event;

            if (event == "clients") {

                repository.data->client.load(&db_calls);

            } else if (event == "airp") {

                repository.data->airp.load(&db_calls);

            } else if (event == "number") {

                repository.data->number.load(&db_calls);

            } else if (event == "outcome") {

                repository.data->outcome.load(&db_calls);

            } else if (event == "prefixlist") {

                repository.data->prefixlist.load(&db_calls);

            } else if (event == "prefixlist_prefix") {

                repository.data->prefixlistPrefix.load(&db_calls);

            } else if (event == "release_reason") {

                repository.data->releaseReason.load(&db_calls);

            } else if (event == "route_case") {

                repository.data->routeCase.load(&db_calls);

            } else if (event == "route_table") {

                repository.data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                repository.data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                repository.data->routeTable.load(&db_calls);

            } else if (event == "route_table_route") {

                repository.data->routeTableRoute.load(&db_calls);

            } else if (event == "routing_report_data") {

                //

            } else if (event == "trunk") {

                repository.data->trunk.load(&db_calls);
                repository.data->trunkByName.load(&db_calls);
                repository.data->trunkByAlias.load(&db_calls);

            } else if (event == "trunk_group") {

                repository.data->trunkGroup.load(&db_calls);

            } else if (event == "trunk_group_item") {

                repository.data->trunkGroupItem.load(&db_calls);

            } else if (event == "trunk_number_preprocessing") {

                repository.data->trunkNumberPreprocessing.load(&db_calls);

            } else if (event == "trunk_priority") {

                repository.data->trunkPriority.load(&db_calls);

            } else if (event == "trunk_rule") {

                repository.data->trunkRule.load(&db_calls);

            } else if (event == "trunk_trunk_rule") {

                repository.data->trunkTrunkRule.load(&db_calls);

            } else if (event == "defs") {

                repository.data->pricelistPrice.load(&db_calls);

            } else if (event == "geo") {

                repository.data->geo.load(&db_calls);

            } else if (event == "prefix") {

                repository.data->geoPrefix.load(&db_calls);

            } else if (event == "mob_prefix") {

                repository.data->mobPrefix.load(&db_calls);

            } else if (event == "instance_settings") {

                repository.data->instanceSettings.load(&db_calls);

            } else if (event == "network_prefix") {

                repository.data->networkPrefix.load(&db_calls);

            } else if (event == "organization") {

                repository.data->organization.load(&db_calls);

            } else if (event == "operator") {

                repository.data->voipOperator.load(&db_calls);

            } else if (event == "pricelist") {

                repository.data->pricelist.load(&db_calls);

            } else if (event == "service_number") {

                repository.data->serviceNumber.load(&db_calls);
                repository.data->activeCounter.load(repository.data->serviceNumber.get(), repository.data->serviceTrunk.get());
                repository.data->serviceNumberByTechNumber.load(&db_calls);

            } else if (event == "service_trunk") {

                repository.data->serviceTrunk.load(&db_calls);
                repository.data->activeCounter.load(repository.data->serviceNumber.get(), repository.data->serviceTrunk.get());

            } else if (event == "service_trunk_settings") {

                repository.data->serviceTrunkSettings.load(&db_calls);

            } else if (event == "service_package") {

                repository.data->serviceNumberPackage.load(&db_calls);
                repository.data->serviceTrunkPackage.load(&db_calls);

            } else if (event == "tariff") {

                repository.data->tariff.load(&db_calls);

            } else if (event == "tariff_change_log") {

                repository.data->tariffChangeLog.load(&db_calls);

            } else if (event == "tariff_package") {

                repository.data->tariffPackage.load(&db_calls);

            } else if (event == "stat_destination_prefixlists") {

                repository.data->statDestinationPrefixlists.load(&db_calls);

            } else if (event == "stat_prefixlist") {

                repository.data->statPrefixlist.load(&db_calls);

            } else if (event == "server") {

                repository.data->server.load(&db_calls);

            } else if (event == "currency_rate") {

                repository.data->currencyRate.load(&db_calls);
            }

            db_calls.exec("DELETE from event.queue WHERE event='" + event + "' and version = '" + lexical_cast<string>(version) + "'");

        }
    } catch (Exception &e) {
        e.addTrace("Loader::run");
        throw e;
    }

    current_event = "";

}

bool ThreadLoader::do_load_data() {
    bool success = true;

    try {
        db_calls.exec("DELETE from event.queue");

        repository.data->loadAll(&db_calls);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_data");
        Log::exception(e);
        success = false;
    }

    return success;
}

bool ThreadLoader::do_load_counters() {
    bool success = true;

    try {

        repository.billingData->loadAll(&db_calls);

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

    db_calls.setCS(app().conf.db_calls);

    init_load_counters_done = false;
    init_load_data_done = false;
}

bool ThreadLoader::hasFullHtml() {
    return true;
}

void ThreadLoader::htmlfull(stringstream &html) {
    this->html(html);

    html << "Current event: <b>" << current_event << "</b><br/>\n";
}
