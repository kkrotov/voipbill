#include "RadiusAuthServerStatus.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"

RadiusAuthServerStatus::RadiusAuthServerStatus() : HealthCheck("RadiusAuthServerStatus") {

}

SystemStatus RadiusAuthServerStatus::getStatus() {

    return healthStatus;
}