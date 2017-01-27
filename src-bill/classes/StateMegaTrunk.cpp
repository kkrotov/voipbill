#include "StateMegaTrunk.h"


StateMegaTrunk::StateMegaTrunk(Repository *repository) {
    this->repository = repository;
    this->trace = nullptr;
}

void StateMegaTrunk::setTrace(stringstream *trace) {
    this->trace = trace;
    repository->trace = trace;
}

void StateMegaTrunk::clearTrace() {
    this->trace = nullptr;
    repository->trace = nullptr;
}

void StateMegaTrunk::prepareFromCdr(Cdr *cdr) {
    if (cdr != nullptr) {
        src_trunk = repository->getTrunkByName(cdr->src_route);

        serviceNumberNumA = repository->getServiceNumber(cdr->src_number);
        serviceNumberNumB = repository->getServiceNumber(cdr->dst_number);

    }
}

void StateMegaTrunk::PhaseCalc() {
    // Расчитываем фазу машрутизации мегатранка

    vector <ServiceTrunk> resultServiceTrunk;

    if (serviceNumberNumB != nullptr) {
        repository->getServiceTrunkByClientID(resultServiceTrunk, serviceNumberNumB->client_account_id);
        for (auto serviceTrunk : resultServiceTrunk) {
            if (time(nullptr) <= serviceTrunk.expire_dt && serviceTrunk.activation_dt <= time(nullptr)) {

                if (serviceTrunk.client_account_id == serviceNumberNumB->client_account_id &&
                    serviceTrunk.term_enabled) {

                    if (!repository->isRegionOnHub(serviceTrunk.server_id)) {
                        // Провеяем, есть на лицевом счете номера B услуга транк,
                        // если такой транк находится в другом регионе, включаем Фазу 1 и перемещаемся в этот регион.

                        destRegion = serviceTrunk.server_id;
                        isPhase1 = true;
                        if (trace != nullptr) {
                            *trace << "INFO|FOUND MEGATRANK|#" << serviceTrunk.trunk_id
                                   << ", SERVICE TRUNK ID: " << serviceTrunk.id << " IN REGION #"
                                   << serviceTrunk.server_id
                                   << ", isPhase1 = true\n";
                        }
                        return;

                    } else {
                        destTrunk = repository->getTrunk(serviceTrunk.trunk_id);
                        if (destTrunk != nullptr) {


                            if (trace != nullptr) {
                                *trace << "INFO|FOUND MEGATRANK|" << destTrunk->name << " ("
                                       << serviceTrunk.trunk_id
                                       << ")" <<
                                       ", SERVICE TRUNK ID: " << serviceTrunk.id
                                       << " AT THIS REGION(HUB). isPhase2 = true\n";
                            }

                            // Если лицевой счет номера B совпадает с лицевым счетом транка в этом регионе, то включаем
                            // фазу 2 и направляем в такой транк.
                            isPhase2 = true;
                        }
                        return;

                    }
                }


            }
        }
    }

    // Если лицевые счете номера А и исходного транка совпадают - и номер А принадлежит другому региону,
    // включаем Фазу 1 и перемещаем звонок в регион номера А.

    resultServiceTrunk.clear();

    if (serviceNumberNumA != nullptr && src_trunk != nullptr) {
        repository->getServiceTrunkByClientID(resultServiceTrunk, serviceNumberNumA->client_account_id);
        for (auto serviceTrunk : resultServiceTrunk) {
            if (time(nullptr) <= serviceTrunk.expire_dt && serviceTrunk.activation_dt <= time(nullptr)) {

                if (serviceTrunk.client_account_id == serviceNumberNumA->client_account_id &&
                    serviceTrunk.orig_enabled && src_trunk->id == serviceTrunk.trunk_id) {

                    destTrunk = repository->getTrunk(serviceTrunk.trunk_id);

                    if (!repository->isRegionOnHub(serviceNumberNumA->server_id) && destTrunk != nullptr) {
                        if (trace != nullptr) {
                            *trace << "INFO|CALL FROM MEGATRANK|" << destTrunk->name << " (" << serviceTrunk.trunk_id
                                   << ") FROM REGION #" << destTrunk->server_id <<
                                   ", DID: " << serviceNumberNumA->did << " FROM REGION #"
                                   << serviceNumberNumA->server_id << ", isPhase1 = true\n";
                        }
                        isPhase1 = true;
                        destRegion = serviceNumberNumA->server_id;
                        return;
                    }
                }
            }
        }
    }

}
