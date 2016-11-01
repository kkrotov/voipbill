#pragma once

#include "Repository.h"

/************************************************************************************************************
 *
 * BillingCall - класс, производящий расчет стоимости плеч звонка
 *   основной метод - calc(Call *call, CallInfo * callInfo, Cdr *cdr)
 *   cdr     -     структура которая содержит исходную информацию для расчета цены и обновления
 *                 счетчиков (А,B номера ; Входящие и Исходящие транки; длительность звонка; дата
 *                 совершения звонка и т.д.). Структура не содержит никакой информации о клиенте, счете,
 *                 тарифе - это все рассчитывается в процессе.
 *   callinfo  -   Структура, которая заполняется в процессе расчета внутри класса. Содержит найденную
 *                 для обсчитываемого звонка из cdr информацию о счете, транке обсчитываемого плеча,
 *                 действующих тарифах, географических параметрах, примененного прайс-листа)
 *   call      -   структура, которая содержит результирующую информацию по проведенному расчету
 *                 для выбранного плеча звонка. Первичная информация заполняется в конструкторе из
 *                 структуры cdr, все остальные поля расчетные. В конструкторе структуры также
 *                указывается для какого плеча производится расчет (параметр bool orig).
 *
 * Класс BillingCall производит расчет цены для orig и term плеча звонка отдельно.
 *
 ************************************************************************************************************/


class BillingCall {
protected:
    Repository *repository;
    Call *call;
    CallInfo *callInfo;
    Cdr *cdr;

    stringstream *trace;

public:
    BillingCall(Repository *repository);
    void setTrace(stringstream *trace);
    void clearTrace();
    void calc(Call *call, CallInfo * callInfo, Cdr *cdr);

protected:
    void calcByTrunk();
    void calcByNumber();
    void calcOrigByNumber();
    void calcTermByNumber();
    void numberPreprocessing();
    void processRedirectNumber();
    void processGeo();
    void processDestinations();
    int getDest(int geo_id);

    void processLineWithoutNumber(Call *call, char *cdr_num);
    long long int getNumber();
    long long int getRemoteNumber();
    char * getRoute();
    char * getRemoteRoute();
    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    bool isUsage7800();

    void setupTrunk();
    void setupEffectiveOrigTrunkSettings();
    void setupEffectiveTermTrunkSettings();
    void setupServiceTrunk();
    void setupServiceNumber();
    void setupServiceTrunkForNumber();
    void setupAccount();
    void setupPricelist(int pricelist_id = 0);
    void setupPrice(long long int numberPrefix = 0);
    void setupBilledTime();
    void setupCost();
    void setupLogTariff();
    void setupMainTariff();
    void setupTariff();
    void setupPackagePricelist();
    void setupFreemin();
    void setupPackagePrepaid();

    bool matchTariffPackageDestination(TariffPackage * tariff);

    void processNNP();


    void setupBilledTimeNNP(NNPAccountTariffLight nnpAccountTariffLight);

    void calcOrigNNPByNumber();


    pair<int, int> setupNNPPackageMinute(vector<NNPAccountTariffLight> &nnpAccountTariffLight,
                                         set<int> &nnpDestinationIds);

    pair<double, int> setupNNPPackagePricelist(vector<NNPAccountTariffLight> &nnpAccountTariffLightList,
                                               set<int> &nnpDestinationIds);

    pair<double, int> setupNNPPackagePrice(vector<NNPAccountTariffLight> &nnpAccountTariffLightList,
                                           set<int> &nnpDestinationIds);

    void setupNNPCost();
};

