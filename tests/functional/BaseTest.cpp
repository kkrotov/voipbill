#include "BaseTest.h"

BaseTest :: BaseTest () {
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURL_WRITE_CALLBACK);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    }
}

BaseTest :: ~BaseTest () {
    if (curl)
        curl_easy_cleanup(curl);
    curl = 0;
}

const string & BaseTest :: getLastError () const {
    return this->lastError;
}

void BaseTest :: resetError () {
    this->lastError.clear();
}