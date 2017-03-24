#pragma once

#include "Repository.h"

/***************************************************************************************************************
 *   NestTrunk - клиентский транк в который собираются номера клиента на разных регионах сети.
 *
 *   Вспомогательный класс для реализации двух-фазовой маршрутизации в RadiusAuthProcessor.
 *
 *   Функционал сбора звонков в клиентский транка. А так же приема звонков из одного транка и доставка из в целевой регион.
 *   Функционал действут для лицевых счетов на которых есть и услуги "номера" и услуги "транк".
 *   Суть функционала:  Клиент имеет несколько услуг телефонов в разных регионах, а так же услугу "транк" .
 *   Задача - собрать звонки в один транк в одном регионе. Транк этот тоже является услугой этого клиента (всегда один).
 *   А так же иметь возможность принять исходящий звонок из такого единого транка, и обеспечить прохождение звонка
 *   через узел региона, которому принадлежит номер (Для корректного СОРМ-ирования).
 *
 *   Случай 1.  Звонк поступает на номер такого клиета.
 *
 *       Фаза 1. - Доставляем звонок на узел, к которому подключен собирающий транк.
 *
 *       Если находится такой лицевой счет, которому принадлежит этот номер, а так же есть услуга "транк".
 *       Если регион подключения транка находится на другом узле, то направляем звонок в транк,
 *       который является "дорогой" в регион транка.
 *
 *       Фаза 2.
 *
 *       Частный случай - сразу наступает фаза 2, если звонк сразу пришел на узел с транком. при поступления
 *       такого звонка на узел с собирающим транком - направляем звонок в собирающий транк.
 *
 *  Случай 2. Звонок приходит из транка , который придлежит лицевому счету, на котором есть и услуга-этого номера А звонка.
 *
 *       Фаза 1. - Доставляем звонок на узел, которому принадлежит этот номер А.
 *
 *       Если находится такой лицевой счет, которому принадлежит этот номер А, а так же есть услуга "транк" оригинационного плеча.
 *       Если регион подключения номера находится на другом узле, то направляем звонок в транк, который является "дорогой" в регион номера.
 *
 *       Фаза 2.
 *
 *       Частный случай - сразу наступает фаза 2, если звонк сразу пришел на узел-подключения номера А. при поступлении
 *       звонка на узел-подключения номера  - выполняем биллингацию обычным порядком.
 *
 */


class StateMegaTrunk {

private:
    Repository *repository;
    Call *call;
    CallInfo *callInfo;
    Cdr *cdr;

    int origRegion = app().conf.instance_id;

    Trunk *src_trunk = nullptr;

    ServiceNumber *serviceNumberNumA = nullptr;
    ServiceNumber *serviceNumberNumB = nullptr;

    bool isPhase1 = false;
    bool isPhase2 = false;
    bool isAuthByNumber = false;

    bool isNeedForceTermAuthByNumber = false;
    bool isNeedForceTermTarifficationSkip = false;

    set<int> simblingRegions;

    stringstream *trace;

    Trunk *destTrunk = nullptr;

    int destRegion = 0;

public:

    StateMegaTrunk(Repository *repository);

    void setTrace(stringstream *trace);

    void clearTrace();

    bool isMegaTrunkPhase1()  {
        return isPhase1;
    };

    bool isMegaTrunkPhase2()  {
        return isPhase2;
    };

    bool isForceOrgAuthByNumber();
    bool isForceTermAuthByNumber();

    Trunk *getDestinationMegaTrunk() {
        return destTrunk;
    }

    bool isTechOrig() {

        return src_trunk!= nullptr && src_trunk->tech_trunk;
    }

    void prepareFromCdr(Cdr *cdr, string aNumber = "", string bNumber = "");
    void PhaseCalc();

    int getDestinationRegion() {
        return destRegion;
    };

    bool isForceOrigTarifficationSkip();


    bool isForceTermTarifficationSkip();

    void setOrigRegion(int  origRegion) {
        this->origRegion = origRegion;
    }
};

