#include "ServiceTrunkPackageList.h"

void ServiceTrunkPackageList::findAll(vector<ServicePackage *> &resultPackages, int service_trunk_id, time_t timestamp, stringstream *trace) {

    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, service_trunk_id, key_service_trunk_id());
        begin = p.first;
        end = p.second;
    }
    {
        end = upper_bound(begin, end, timestamp, key_activation_dt());
    }

    if (begin < end) {
        for (auto it = begin; it != end; ++it) {
            ServicePackage * servicePackage = &*it;
            if (servicePackage->expire_dt > timestamp) {
                resultPackages.push_back(servicePackage);
            }
        }
    }

    if (trace != nullptr) {
        if (resultPackages.size() == 0) {
            *trace << "NOT FOUND|SERVICE PACKAGE|BY SERVICE_TRUNK_ID '" << service_trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        } else {
            *trace << "FOUND|SERVICE PACKAGE|BY SERVICE_TRUNK_ID '" << service_trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        }
        for (auto servicePackage : resultPackages) {
            *trace << "||";
            servicePackage->dump(*trace);
            *trace << "\n";
        }
    }
}
