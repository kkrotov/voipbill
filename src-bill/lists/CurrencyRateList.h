#pragma once

#include "../classes/ObjList.h"
#include "../models/CurrencyRate.h"

class CurrencyRateList : public ObjList<CurrencyRate> {

    string sql(BDb * db) {
        time_t moscowDay = get_tday(time(nullptr), 3);
        std::string moscowDayStr = string_date(moscowDay);

        return "select currency, \"date\", rate from billing.currency_rate "
            "where \"date\" > '" + moscowDayStr + "' or id in "
            "  ( "
            "      select r.id from billing.currency_rate r "
            "          inner join ( "
            "              select distinct(currency), max(\"date\") \"date\" from billing.currency_rate "
            "              where \"date\" <= '" + moscowDayStr + "' "
            "              group by currency "
            "              ) cd "
            "          on (cd.currency = r.currency and cd.\"date\" = r.\"date\") "
            "  ) "
            "order by \"date\" desc";
    }

    inline void parse_item(BDbResult &row, CurrencyRate * item) {
        row.fill_cs(0, item->currency, sizeof(item->currency));
        item->date = row.get_ll(1);
        item->rate = row.get_d(2);
    }

public:
    const CurrencyRate * find(const char * currency_id) const {
        if (!currency_id || !currency_id[0]) {
            return 0;
        }

        time_t moscowDay = get_tday(time(nullptr), 3);

        // Список отсортирован по дате, от большей к меньшей.
        for (auto &row : this->data) {
            if (0 == strncmp(row.currency, currency_id, 4)) {
                // Учитываем только данные по курсу указанной валюты
                if (moscowDay >= row.date) {
                    return & row;
                }
            }
        }

        return 0;
    }
};

