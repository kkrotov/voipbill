#include "classes/AppBill.h"

#include <iostream>

int main(int argc, char* argv[]) {
/*
    struct Xxx {
        int id;
        int f1;
        int t1;
        int t2;
    };

    int n = 10;
    vector<Xxx> m;
    m.push_back(Xxx {1, 1, 11, 20});
    m.push_back(Xxx {2, 1, 21, 30});
    m.push_back(Xxx {3, 1, 31, 40});
    m.push_back(Xxx {4, 1, 41, 50});

    m.push_back(Xxx {5, 2, 11, 20});
    m.push_back(Xxx {6, 2, 21, 30});
    m.push_back(Xxx {7, 2, 31, 40});
    m.push_back(Xxx {8, 2, 41, 50});


    for (Xxx &x : m) {
        cout << x.id << "\t\t" << x.f1 << "\t\t" << x.t1 << "\t\t" << x.t2 << "\t\t" << endl;
    }

    struct key1 {
        bool operator() (const Xxx & left, int value) {
            return left.f1 < value;
        }
        bool operator() (int value, const Xxx & right) {
            return value < right.f1;
        }
    };

    struct key2 {
        bool operator() (const Xxx & left, time_t timestamp) {
            return left.t2 < timestamp;
        }
        bool operator() (time_t timestamp, const Xxx & right) {
            return timestamp < right.t1;
        }
    };

    auto i1 = m.begin();
    auto i2 = m.end();
    {
        auto p = equal_range(i1, i2, 2, key1());
        i1 = p.first;
        i2 = p.second;
        if (i1 != m.end()) {
            cout << "low  " << (*i1).id << " " << (*i1).f1 << " " << (*i1).t1 << " " << (*i1).t2 << endl;
        } else {
            cout << "low  not found" << endl;
        }
        if (i2 != m.end()) {
            cout << "high " << (*i2).id << " " << (*i2).f1 << " " << (*i2).t1 << " " << (*i2).t2 << endl;
        } else {
            cout << "high not found" << endl;
        }
    }
    {
        auto p = equal_range(i1, i2, 25, key2());
        i1 = p.first;
        i2 = p.second;
        if (i1 != m.end()) {
            cout << "low  " << (*i1).id << " " << (*i1).f1 << " " << (*i1).t1 << " " << (*i1).t2 << endl;
        } else {
            cout << "low  not found" << endl;
        }
        if (i2 != m.end()) {
            cout << "high " << (*i2).id << " " << (*i2).f1 << " " << (*i2).t1 << " " << (*i2).t2 << endl;
        } else {
            cout << "high not found" << endl;
        }
    }

    return 0;
*/
    if (!app().init(argc, argv))
        return 1;

    app().run();

    return 0;

}

