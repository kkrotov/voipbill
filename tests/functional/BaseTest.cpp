#include "BaseTest.h"

BaseTest :: BaseTest () {

}

const string & BaseTest :: getLastError () const {
    return this->lastError;
}

void BaseTest :: resetError () {
    this->lastError.clear();
}