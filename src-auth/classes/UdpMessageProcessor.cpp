#include <vector>

#include "../../src/common.h"
#include "../../src/classes/Log.h"
#include "UdpMessageProcessor.h"

UdpMessageProcessor::UdpMessageProcessor(const string &message) {
    this->message = message;
}

void UdpMessageProcessor::parseRequest() {

    vector<string> parameters;
    split(parameters, message, boost::algorithm::is_any_of(";"));

    string calling_station_id;
    string called_station_id;

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        vector<string> pair;
        split(pair, *it, boost::algorithm::is_any_of(":"));

        if (pair.size() == 2) {
            string name = pair.at(0);
            string value = pair.at(1);

            if (name == "calling") {
                callingStationId = value;
            } else if (name == "called") {
                calledStationId = value;
            }
        }
    }
}

bool UdpMessageProcessor::validateRequest() {

    if (callingStationId == "") {
        Log::error("Udp request validation: bad calling: " + message);
        return false;
    }

    if (calledStationId == "") {
        Log::error("Udp request validation: bad called: " + message);
        return false;
    }

    return true;

}

string UdpMessageProcessor::process() {

    parseRequest();

    if (validateRequest()) {
        return "SUCCESS RESPONSE";
    } else {
        return "ERROR RESPONSE";
    }

}