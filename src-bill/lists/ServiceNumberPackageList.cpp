#include "ServiceNumberPackageList.h"

void ServiceNumberPackageList::findAll(vector<ServicePackage *> &resultPackages, int service_number_id, time_t timestamp, stringstream *trace) {

    auto begin = this->data.begin();
    auto end = this->data.end();
    {
        auto p = equal_range(begin, end, service_number_id, key_service_number_id());
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
            *trace << "NOT FOUND|SERVICE PACKAGE|BY SERVICE_NUMBER_ID '" << service_number_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        } else {
            *trace << "FOUND|SERVICE PACKAGE|BY SERVICE_NUMBER_ID '" << service_number_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
        }
        for (auto servicePackage : resultPackages) {
            *trace << "||";
            servicePackage->dump(*trace);
            *trace << "\n";
        }
    }
}
