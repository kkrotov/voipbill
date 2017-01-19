#include "ServiceTrunkList.h"

ServiceTrunk * ServiceTrunkList::find(int trunk_id, time_t timestamp, stringstream *trace) {
    vector<ServiceTrunk *> resultTrunks;
    findAll(resultTrunks, trunk_id, timestamp, trace);
    if (resultTrunks.size() > 0) {
        return resultTrunks.at(0);
    } else {
        return nullptr;
    }
}

void ServiceTrunkList::findAll(vector<ServiceTrunk *> &resultTrunks, int trunk_id, time_t timestamp, stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, trunk_id, key_trunk_id());
        begin = p.first;
        end = p.second;
    }
    {
        end = upper_bound(begin, end, timestamp, key_activation_dt());
    }

    if (begin < end) {
        for (auto it = begin; it != end; ++it) {
            ServiceTrunk * serviceTrunk = &*it;
            if (serviceTrunk->expire_dt >= timestamp) {
                resultTrunks.push_back(serviceTrunk);
            }
        }
    }

    if (trace != nullptr) {
        if (resultTrunks.size() == 0) {
            *trace << "NOT FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        } else {
            *trace << "FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        }
        for (auto serviceTrunk : resultTrunks) {
            *trace << "||";
            serviceTrunk->dump(*trace);
            *trace << "\n";
        }
    }
}

void ServiceTrunkList::findAllByClientID(vector<ServiceTrunk> &resultServiceTrunk, int client_id,
                                          stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();

    bool f = false;

    for (auto it = begin; it != end; ++it) {
        ServiceTrunk serviceTrunk = *it;
        if (serviceTrunk.client_account_id == client_id) {
            f = true;
            resultServiceTrunk.push_back(serviceTrunk);
            if (trace != nullptr) {
                *trace << "FOUND|SERVICETRUNK|BY CLIENT_ID '" << client_id << "'\n";
                *trace << "||";
                serviceTrunk.dump(*trace);
                *trace << "\n";
            }
        }
    }
    if (!f) {
        if (trace != nullptr) {
            *trace << "NOT FOUND|SERVICETRUNK|BY CLIENT_ID '" << client_id << "'\n";
        }
    }
}