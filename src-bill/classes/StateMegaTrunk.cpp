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
    if(cdr != nullptr) {
        src_trunk = repository->getTrunkByName(cdr->src_route);

        if(src_trunk != nullptr) {
            serviceTrunkSrc = repository->getServiceTrunk(src_trunk->id);
        }

        serviceNumberNumA = repository->getServiceNumber(cdr->src_number);
        serviceNumberNumB = repository->getServiceNumber(cdr->dst_number);

    }
}

void StateMegaTrunk::PhaseCalc() {
    // Расчитываем фазу машрутизации мегатранка

    vector<ServiceTrunk> resultServiceTrunk;

    if( serviceNumberNumB!= nullptr ) {
        repository->getServiceTrunkByClientID(resultServiceTrunk, serviceNumberNumB->client_account_id);
        for(auto serviceTrunk : resultServiceTrunk) {
            if(serviceTrunk.client_account_id == serviceNumberNumB->client_account_id && serviceTrunk.term_enabled &&
               serviceTrunk.activation_dt <= time(nullptr) && time(nullptr) <= serviceTrunk.expire_dt) {

                if(serviceNumberNumB->server_id != serviceTrunk.server_id ) {
                    // Провеяем, есть на лицевом счете номера B услуга транк,
                    // если такой транк находится в другом регионе, включаем Фазу 1 и перемещаемся в этот регион.
                    destRegion = serviceTrunk.server_id;
                    isPhase1 = true;
                    return;

                } else {
                    destTrunk = repository->getTrunk(serviceTrunk.trunk_id);
                    if(destTrunk!= nullptr)
                    {
                        // Если лицевой счет номера B совпадает с лицевым счетом транка в этом регионе, то включаем
                        // фазу 2 и направляем в такой транк.
                        isPhase2 = true;
                        return;
                    }

                }

            }
        }
    }

    // Если лицевые счете номера А и исходного транка совпадают - и номер А принадлежит другому региону,
    // включаем Фазу 1 и перемещаем звонок в регион номера А.

    if(serviceNumberNumA != nullptr && serviceTrunkSrc != nullptr) {
        if(serviceNumberNumA->client_account_id == serviceTrunkSrc->client_account_id &&
           serviceNumberNumA->server_id !=  serviceTrunkSrc->server_id ) {
            isPhase1 = true;
            destRegion =serviceNumberNumA->server_id;
            return;
        }
    }

}
