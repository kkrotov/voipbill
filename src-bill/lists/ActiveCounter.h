#pragma once

#include "../classes/ObjList.h"
#include "ServiceNumberList.h"
#include "ServiceTrunkList.h"
#include "../models/Client.h"
#include <set>
#include <map>
#include <vector>

using namespace std;

class ActiveCounter : public BaseObjList {
public:
    set<int> activeNumbers;
    set<int> activeTrunks;
    set<int> activeClients;
    map<int, map<int, ServiceNumber>> clientNumbers;
    map<int, map<int, ServiceTrunk>> clientTrunks;

    void load(BDb * db) {
    }

    void load(shared_ptr<ServiceNumberList> serviceNumberList, shared_ptr<ServiceTrunkList> serviceTrunkList) {

        time_t now = time(nullptr);

        for (size_t j = 0; j < serviceNumberList->size(); j++) {
            auto serviceNumber = serviceNumberList->get(j);
            if (serviceNumber->activation_dt > now || serviceNumber->expire_dt < now || !serviceNumber->is_local) {
                continue;
            }

            activeNumbers.insert(serviceNumber->id);
            activeClients.insert(serviceNumber->client_account_id);

            map<int, ServiceNumber> &numbers = clientNumbers[serviceNumber->client_account_id];
            numbers.insert(make_pair(serviceNumber->id, *serviceNumber));
        }

        for (size_t j = 0; j < serviceTrunkList->size(); j++) {
            auto serviceTrunk = serviceTrunkList->get(j);
            if (serviceTrunk->activation_dt > now || serviceTrunk->expire_dt < now) {
                continue;
            }

            activeTrunks.insert(serviceTrunk->id);
            activeClients.insert(serviceTrunk->client_account_id);

            map<int, ServiceTrunk> &trunks = clientTrunks[serviceTrunk->client_account_id];
            trunks.insert(make_pair(serviceTrunk->id, *serviceTrunk));
        }

        loadtime = time(NULL);
    }

    size_t size() {
        return activeClients.size();
    }
};


