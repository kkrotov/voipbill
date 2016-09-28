#include "ServiceNumberList.h"

ServiceNumber *ServiceNumberList::find(long long int numberPrefix, time_t timestamp, stringstream *trace) {

    char did[20];
    sprintf(did, "%lld", numberPrefix);

    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, &did[0], key_did());
        begin = p.first;
        end = p.second;
    }
    {
        end = upper_bound(begin, end, timestamp, key_activation_dt());
    }

    ServiceNumber *result = nullptr;
    if (begin < end) {
        for (auto it = begin; it != end; ++it) {
            ServiceNumber *serviceNumber = &*it;
            if (serviceNumber->expire_dt >= timestamp) {
                result = serviceNumber;
                break;
            }
        }
    }

    if (trace != nullptr) {

        if (result != nullptr) {
            *trace << "FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << string_time(timestamp) << "'" <<
            "\n";
            *trace << "||";
            result->dump(*trace);
            *trace << "\n";
        } else {
            *trace << "NOT FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << string_time(timestamp) <<
            "'" << "\n";
        }
    }

    return result;
}

void ServiceNumberList::findAllByClientID(vector<ServiceNumber> &resultServiceNumber, int client_id,
                                          stringstream *trace) {
    auto begin = this->data.begin();
    auto end = this->data.end();

    bool f = false;

    for (auto it = begin; it != end; ++it) {
        ServiceNumber serviceNumber = *it;
        if (serviceNumber.client_account_id == client_id) {
            f = true;
            resultServiceNumber.push_back(serviceNumber);
            if (trace != nullptr) {
                *trace << "FOUND|SERVICENUMBER|BY CLIENT_ID '" << client_id << "'\n";
                *trace << "||";
                serviceNumber.dump(*trace);
                *trace << "\n";
            }
        }
    }
    if (!f) {
        if (trace != nullptr) {
            *trace << "NOT FOUND|SERVICENUMBER|BY CLIENT_ID '" << client_id << "'\n";
        }
    }
}