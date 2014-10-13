#pragma once

#include "../../src/classes/BDb.h"

class ConfigVersionDao {
private:
    BDb * db;

public:

    ConfigVersionDao(BDb * db) {
        this->db = db;
    }

    pConfigVersion getCurrentVersion() {
        BDbResult res =
                db->query(" \
                    select \
                        id, name, \
                        low_balance_outcome_id, blocked_outcome_id, calling_station_id_for_line_without_number \
                    from auth.config_version \
                    limit 1 \
                ");
        if (res.next()) {

            pConfigVersion version = new ConfigVersion();
            version->id = res.get_i(0);
            res.fill_cs(1, version->name, sizeof (version->name));
            version->low_balance_outcome_id = res.get_i(2);
            version->blocked_outcome_id = res.get_i(3);
            res.fill_cs(4, version->calling_station_id_for_line_without_number, sizeof (version->calling_station_id_for_line_without_number));

            return version;
        } else {
            return 0;
        }
    }
};

typedef ConfigVersionDao * pConfigVersionDao;