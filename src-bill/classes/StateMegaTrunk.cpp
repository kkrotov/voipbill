//
// Created by nedlosster on 20.01.17.
//

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

bool StateMegaTrunk::isFromMegaTrunkPhase1() {
    return false;
}

bool StateMegaTrunk::isFromMegaTrunkPhase2() {
    return false;
}

bool StateMegaTrunk::isToMegaTrunkPhase1() {
    return false;
}

bool StateMegaTrunk::isToMegaTrunkPhase2() {
    return false;
}

void StateMegaTrunk::prepareFromCdr(Cdr *cdr) {

}

void StateMegaTrunk::PhaseCalc() {
// Расчитываем фазу машрутизации мегатранка

}

bool StateMegaTrunk::isMegaPhase1(){
    if(isFromMegaTrunkPhase1()) {
        return true;
    }

    if(isToMegaTrunkPhase1()) {

        return true;
    }
    return false;
}

bool StateMegaTrunk::isMegaPhase2(){
    if(isToMegaTrunkPhase2()) {
        return true;
    }
    return false;
}

int StateMegaTrunk::getDestinationRegion() {
    return 0;
}

Trunk *StateMegaTrunk::getDestinationMegaTrunk() {
    return nullptr;
}


/*************************************************************************************************
 * Реализация механизма TrunkBeam - подключение клиента с множеством  номеров через один транк "Beam"
 *
 * isNeedTransferToTrunkBeam - проверяет необходимость выполнения "Фазы 1" - перемещение звонка
 *                             в регион подключения транка-"Beam" при входящем звонке на номер клиента,
 *                             либо перемещения звонка в регион присоединения номера А, при исходящем звонке, когда
 *                             мы приняли звонок из транка "Beam" клиента.
 *
 *
 * @param call
 * @return pair.first - нужно перемещать pait.second - номер региона в который нужно перемещать
 */

/*

pair<bool, int> RadiusAuthProcessor::isNeedTransferToTrunkBeam(Call &call) {
    vector<ServiceTrunk> resultServiceTrunk;

    repository.getServiceTrunkByClientID(resultServiceTrunk,
                                         call.account_id); // Проверяем наличие услуги "Транк" на лицевом счете номера B

    if (resultServiceTrunk.size() > 0) {
        ServiceTrunk trunk = *resultServiceTrunk.begin();

        if (trunk.server_id != call.server_id) { //

        }


    }
*/

