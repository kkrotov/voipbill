#pragma once

#include <string>

using namespace std;

class RadiusAuthProcessorBill {
public:

    RadiusAuthProcessorBill(const string &aNumber, const string &bNumber, const string &trunkName);

    string process();

private:
    string aNumber;
    string bNumber;
    string trunkName;

    void prepareCall();
    void calculateCall();
    string analyzeCall();
};

