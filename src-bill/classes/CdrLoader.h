#pragma once

#include "../data/DataBillingContainer.h"

class CdrLoader {
private:
    BDb * db_calls;
    DataBillingContainer * billingData;
public:

    void setDb(BDb * db_calls) {
        this->db_calls = db_calls;
    }

    void setBillingData(DataBillingContainer * billingData) {
        this->billingData = billingData;
    }

    bool load(const size_t rows_per_request) {
        const int cdr_max_queue_length = 50000;

        if (billingData->cdrsWaitProcessing.size() >= cdr_max_queue_length) {
            return true;
        }

        string query =
                "	select " \
            "       id, " \
            "       connect_time, " \
            "       session_time, " \
            "       src_number, " \
            "       dst_number," \
            "       redirect_number," \
            "       src_route, " \
            "       dst_route," \
            "       src_noa, " \
            "       dst_noa," \
            "       call_id" \
            "	from calls_cdr.cdr " \
            "	where " \
            "       id > '" + lexical_cast<string>(billingData->lastCdrId) + "' " \
            "	order by id " \
            "	limit " + lexical_cast<string>(rows_per_request);

        BDbResult res = db_calls->query(query);
        if (res.size() > 0) {

            while (res.next()) {

                Cdr cdr;
                cdr.id = res.get_ll(0);
                cdr.connect_time = parseDateTime(res.get(1));
                cdr.session_time = res.get_i(2);
                strcpy((char *) &cdr.src_number, res.get(3));
                strcpy((char *) &cdr.dst_number, res.get(4));
                strcpy((char *) &cdr.redirect_number, res.get(5));
                strcpy((char *) &cdr.src_route, res.get(6));
                strcpy((char *) &cdr.dst_route, res.get(7));
                cdr.src_noa = res.get_i(8);
                cdr.dst_noa = res.get_i(9);
                cdr.call_id = res.get_ll(10);

                lock_guard<Spinlock> guard(billingData->callsWaitSavingLock);
                billingData->cdrsWaitProcessing.push_back(cdr);

                billingData->lastCdrId = cdr.id;
                billingData->lastCdrTime = cdr.connect_time;
            }
        }
        return res.size() < rows_per_request;
    }
};