#pragma once

#include "../classes/ObjList.h"
#include "../classes/Spinlock.h"
#include "../models/StatsPackage.h"
#include "../models/CallInfo.h"
#include <map>
using namespace std;

class StatsPackageCounter : public BaseObjList {
private:
    int lastPackageStatId = 0;
public:
    long long int lastSaveCallTime;
    unsigned long long int marker = 0;
    Spinlock lock;

    map<int, list<int>> packages;
    map<int, StatsPackage> packageStats;
    map<int, unsigned long long int> changes;

    void load(BDb * db) {

        loadtime = time(NULL);

        {
            BDbResult res = db->query("   select max(id) from billing.stats_package p ");
            lastPackageStatId = res.next() ? res.get_i(0) : 1;
        }

        {
            time_t filterFrom = lastSaveCallTime;
            if (filterFrom > 0) {
                filterFrom = filterFrom - 86400 * 60;
            }

            BDbResult res = db->query(
                "   select id, package_id, used_seconds, paid_seconds, extract(epoch from activation_dt), extract(epoch from expire_dt), min_call_id, max_call_id " \
                "   from billing.stats_package p " \
                "   where activation_dt >= '" + string_time(filterFrom) + "'" \
                "   order by package_id asc, activation_dt asc "
            );
            while (res.next()) {
                StatsPackage package;
                package.id = res.get_i(0);
                package.package_id = res.get_i(1);
                package.used_seconds = res.get_i(2);
                package.paid_seconds = res.get_i(3);
                package.activation_dt = res.get_ll(4);
                package.expire_dt = res.get_ll(5);
                package.min_call_id = res.get_ll(6);
                package.max_call_id = res.get_ll(7);

                packageStats.insert(make_pair(package.id, package));
                packages[package.package_id].push_back(package.id);
            }
        }

    }

    bool checkAvailableSeconds(int package_id, time_t connect_time, int &availableSeconds) {
        lock_guard<Spinlock> guard(lock);

        availableSeconds = 0;

        list<int> &packageList = packages[package_id];
        for (int packageId : packageList) {

            StatsPackage &package = packageStats.at(packageId);

            if (package.activation_dt > connect_time) continue;
            if (package.expire_dt < connect_time) continue;

            int seconds = package.paid_seconds - package.used_seconds;
            if (seconds > 0) {
                availableSeconds = seconds;
            }

            return true;
        }

        return false;
    }

    void add(CallInfo * callInfo) {

        if (callInfo->call->number_service_id == 0 || callInfo->call->service_package_id == 0 || callInfo->call->package_time == 0) {
            return;
        }

        lock_guard<Spinlock> guard(lock);

        list<int> &packageList = packages[callInfo->servicePackagePrepaid->id];
        for (int packageId : packageList) {

            StatsPackage &package = packageStats.at(packageId);

            if (package.activation_dt > callInfo->call->connect_time) continue;
            if (package.expire_dt < callInfo->call->connect_time) continue;

            package.used_seconds += callInfo->call->package_time;
            package.max_call_id = callInfo->call->id;

            changes[package.id] = ++marker;

            return;
        }

        createPackageStats(callInfo);


    }

    size_t size() {
        return packageStats.size();
    }


    void getChanges(unsigned long long int &destMarker, vector<int> &destChanges) {
        lock_guard<Spinlock> guard(lock);

        destMarker = marker;

        destChanges.reserve(changes.size());
        for (auto pair : changes) {
            destChanges.push_back(pair.first);
        }
    }

    void fixChanges(unsigned long long int old_marker) {
        lock_guard<Spinlock> guard(lock);

        auto it = changes.begin();
        while (it != changes.end()) {
            if (it->second <= old_marker) {
                changes.erase(it++);
            } else {
                ++it;
            }
        }
    }

private:

    void createPackageStats(CallInfo * callInfo) {

        short timezone_offset = 0;

        time_t activation_dt = get_tmonth(callInfo->call->connect_time, timezone_offset);
        time_t expire_dt = get_tmonth_end(callInfo->call->connect_time, timezone_offset);

        if (activation_dt < callInfo->servicePackagePrepaid->activation_dt) {
            activation_dt = callInfo->servicePackagePrepaid->activation_dt;
        }

        if (expire_dt > callInfo->servicePackagePrepaid->expire_dt) {
            expire_dt = callInfo->servicePackagePrepaid->expire_dt;
        }

        lastPackageStatId += 1;

        StatsPackage package;
        package.id = lastPackageStatId;
        package.package_id = callInfo->servicePackagePrepaid->id;
        package.used_seconds = callInfo->call->package_time;
        package.paid_seconds = callInfo->tariffPackagePrepaid->getPrepaidSeconds();
        package.activation_dt = activation_dt;
        package.expire_dt = expire_dt;
        package.min_call_id = callInfo->call->id;
        package.max_call_id = callInfo->call->id;

        packageStats.insert(make_pair(package.id, package));
        packages[package.package_id].push_back(package.id);

        changes[package.id] = ++marker;
    }

};


