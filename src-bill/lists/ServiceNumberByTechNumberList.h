#pragma once

#include "../classes/ObjList.h"
#include "../models/ServiceNumber.h"
#include "../classes/AppBill.h"

class ServiceNumberByTechNumberList : public ObjList<ServiceNumber> {
protected:

    string sql(BDb * db) {
        return "    select did, id, client_account_id, lines_count, extract(epoch from activation_dt), extract(epoch from expire_dt), tech_number, tech_number_operator_id " \
            "       from billing.service_number " \
            "       where server_id in " + app().conf.get_sql_regions_for_load_list_list() + " " \
            "       and tech_number is not null " \
            "       order by tech_number asc, activation_dt asc ";
        //  and expire_dt > now()
    }

    inline void parse_item(BDbResult &row, ServiceNumber * item) {
        row.fill_cs(0, item->did, sizeof(item->did));
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->lines_count = row.get_i(3);
        item->activation_dt = row.get_ll(4);
        item->expire_dt = row.get_ll(5);
        row.fill_cs(6, item->tech_number, sizeof(item->tech_number));
        item->tech_number_operator_id = row.get_i(7);
    }

    struct key_activation_dt {
        bool operator() (const ServiceNumber & left, time_t activation_dt) {
            return left.activation_dt < activation_dt;
        }
        bool operator() (time_t activation_dt, const ServiceNumber & right) {
            return activation_dt < right.activation_dt;
        }
    };

    struct key_tech_number {
        bool operator() (const ServiceNumber & left, const char * tech_number) {
            if (left.tech_number[0]) {
                return strcmp(left.tech_number, tech_number) < 0;
            } else {
                return true;
            }
        }
        bool operator() (const char * tech_number, const ServiceNumber & right) {
            if (right.tech_number[0]) {
                return strcmp(tech_number, right.tech_number) < 0;
            } else {
                return true;
            }
        }
    };

public: // Внимание, тут нужно учитывать регион
    ServiceNumber * find(long long int technicalNumber, time_t timestamp, stringstream *trace = nullptr) {
        char tech_number[20];
        sprintf(tech_number, "%lld", technicalNumber);

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, &tech_number[0], key_tech_number());
            begin = p.first;
            end = p.second;
        }
        {
            end = upper_bound(begin, end, timestamp, key_activation_dt());
        }

        ServiceNumber * result = nullptr;
        if (begin < end) {
            for (auto it = begin; it != end; ++it) {
                ServiceNumber * serviceNumber = &*it;
                if (serviceNumber->expire_dt >= timestamp) {
                    result = serviceNumber;
                    break;
                }
            }
        }

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|SERVICE NUMBER|BY TECHNICAL NUMBER '" << technicalNumber << "', TIME '" << string_time(timestamp) << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|SERVICE NUMBER|BY TECHNICAL NUMBER '" << technicalNumber << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }

        return result;
    }
};
