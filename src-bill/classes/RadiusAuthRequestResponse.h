#pragma once

struct RadiusAuthRequest {
    string trunkName;
    string srcNumber;
    string dstNumber;
    string redirectNumber;
    string callingPartyCategory;
    int srcNoa = 3;
    int dstNoa = 3;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_name: " << trunkName << ", ";
        trace << "src_number: " << srcNumber << ", ";
        trace << "dst_number: " << dstNumber << ", ";
        trace << "redirect_number: " << redirectNumber << ", ";
        trace << "src_noa: " << srcNoa << ", ";
        trace << "dst_noa: " << dstNoa << ", ";
        if (callingPartyCategory.size() > 0)
            trace << "calling_party_category: " << callingPartyCategory << ", ";
        trace << ")";
    }
};

struct RadiusAuthResponse {
    Timer timer;
    bool accept;
    string srcNumber;
    string dstNumber;
    string routeCase;
    string releaseReason;
    string airp;
    string error;

    void setRouteCase(string routeCase) {
        this->accept = true;
        this->routeCase = routeCase;
    }

    void setAirp(string airp) {
        this->accept = true;
        this->airp = airp;
    }

    void setReleaseReason(string releaseReason) {
        this->accept = true;
        this->releaseReason = releaseReason;
    }

    void setAccept() {
        this->accept = true;
    }

    void setReject() {
        this->accept = false;
        this->srcNumber = "";
        this->dstNumber = "";
    }
};
