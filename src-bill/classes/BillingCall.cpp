#include "BillingCall.h"
#include "../classes/CalcException.h"

BillingCall::BillingCall(Repository *repository) {
    this->repository = repository;
    this->trace = nullptr;
}

void BillingCall::setTrace(stringstream *trace) {
    this->trace = trace;
    repository->trace = trace;
}

void BillingCall::clearTrace() {
    this->trace = nullptr;
    repository->trace = nullptr;
}

/*******************************************************************************************************************
 *  Расчет плечей звонка из cdr, выбор расчитываемого плеча происходит параметром call->orig
 *
 *  cdr     -     структура которая содержит исходную информацию для расчета цены и обновления
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
 *  */

void BillingCall::calc(Call *call, CallInfo *callInfo, Cdr *cdr) {
    try {
        this->cdr = cdr;
        this->call = call;
        this->callInfo = callInfo;
        this->callInfo->call = call;

        setupTrunk();                   // Загружает в callInfo->trunk информацию по транку обсчитываемого плеча.
        // При этом так же происходит вычисление действующей схемы авторизации
        // для расчета - по номеру или по транку.

        processSignalingCallId();

        if (call->orig) {               // При расчете оригинационного плеча
            numberPreprocessing();      // на основании значений noa и правил препроцессинга номера для транка
            // (таблица auth.trunk_number_preprocessing) дополняет номера необходимыми
            // префиксами слева, приводя их к 11-значному виду.
            processRedirectNumber();    // происходит подстановка в поле src_number из поля redirect number при
            // наличии. В случае наличие на транке флага orig_redirect_number_7800 выполняется подмена dst_номера
            // на технический на основании таблицы  billing.service_number
        }

        processGeo();                   // в структуру call рассчитываются поля  geo_mob и geo_id, geo_operator_id
        // флаг вызова на мобильный,  регион и оператор - на противоположном плече?
        // (для org плеча - рассчитываем гео-параметры  Б номера,
        // для term плеча - для А- номера)
        processDestinations();          // в структуру call рассчитывается поля mob и destination_id
        // флаг звонка на мобильный и тип звонка (локал,внутризоновый, МГ, МН)

        processNNP();                   // Расчитываем nnp-параметры для плеча.

        if (callInfo->trunk->auth_by_number) {
            calcByNumber();             // Дальше производятся тарификация плеча по схеме "авторизация по номеру"
        } else {
            calcByTrunk();              // Дальше производятся тарификация плеча по схеме "авторизация по транку"
        }
    } catch (CalcException &e) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE " << e.message << "\n";
        }
        return;
    }
}


/********************************************************************************************************************
 *  Реализация тарификации плеча по схеме "авторизация по транку"
 */


void BillingCall::calcByTrunk() {


    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY TRUNK" << "\n";
    }


    if (call->orig) {
        setupEffectiveOrigTrunkSettings();  // Вычисляем минимальную цену в присоединенных прайслистах на входящем транке
    } else {
        setupEffectiveTermTrunkSettings();  // Вычисляем минимальную цену в присоединенных прайслистах на исходящем транке
    }

    setupServiceTrunk();                    // Присоединение данных по услуге Транк на плече из структуры callInfo в call

    setupAccount();                         // в структуре callinfo заполняется структура account
    // (информация о лицевом счете обсчитываемого плеча)
    if (call->account_version == CALL_ACCOUNT_VERSION_4) {
        if (call->orig) {
            setupPackagePricelist();            // поисх подходящего пакета для обсчета оригинационного плеча, если находим, используем его
        }
        setupPricelist();                       // присоединяем в callInfo->pricelist прайслист по pricelist_id
        // при расчете с авторизацией по транку прайслист должен быть
        // выбран ранее. Из пакета?

        setupPrice();                           //  Расчитываем по ранее выбранному прайслисту и префиксу
        // номера цену звонка на плече

        setupBilledTime();                      //  вычисляется тарифицируемая длительность звонка согласно настройкам
        // (бесплатные секунды, тарификация по мин/по сек) действующего
        // тарифа для расчитываемого плеча.

        if (call->orig) {
            setupPackagePrepaid();              // на оигинационном плече сохраняем в call секунды взятые из подходящего
            // подключенного пакета, ссылку на сам пакет тоже сохраняем в call
        }

        // В случае расчета терминационного плеча в поля call->interconnect_rate и call->interconnect_cost сохраняем
        // дополнительную  цену и стоимость интерконнекта (для случая локального и МН МГ вызовов).

        if (!call->orig && callInfo->pricelist->initiate_zona_cost > 0.00001 && call->destination_id == 0) {
            call->interconnect_rate = callInfo->pricelist->initiate_zona_cost;
            call->interconnect_cost = call->billed_time * call->interconnect_rate / 60.0;
        }
        if (!call->orig && callInfo->pricelist->initiate_mgmn_cost > 0.00001 && call->destination_id > 0) {
            call->interconnect_rate = callInfo->pricelist->initiate_mgmn_cost;
            call->interconnect_cost = call->billed_time * call->interconnect_rate / 60.0;
        }
        setupCost();                            // Расчет стоимости плеча. С учетом остатка по найденому пакету
    } else if (call->account_version == CALL_ACCOUNT_VERSION_5) {

        setupBilledTimeNNP(callInfo->nnpPackage);

        setupNNPCost();

    } else {
        throw CalcException("UNKNOW ACCOUNT VERSION");
    }

}

/********************************************************************************************************************
 *  Дальше производятся тарификация плеча по схеме "авторизация по номеру"
 */

void BillingCall::calcByNumber() {

    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY NUMBER" << "\n";
    }

    if (call->orig && atoll(cdr->redirect_number) == 0) {
        processLineWithoutNumber(call,
                                 cdr->src_number); // на оригинационном плече отрабатывается случай выполнения звонка
        // c линии без номера

    }

    setupServiceNumber();                   // структуре callinfo заполняется структура ServiceNumber для
    // обсчитываемого плеча ( инфомация об услуге номер,
    // т.к. идет авторизация по номеру).

    setupServiceTrunkForNumber();           // в структуре callinfo заполняется структура ServiceTrunkForNumber
    // для обсчитываемого плеча (информация о транке обсчитываемого плеча,
    //  проверяем ситуацию совпадения клиента для транка и номера для
    // не наших клиентов, что бы не приходили звонки с левых номеров?)

    setupAccount();                         // в структуре callinfo заполняется структура account
    // (информация о лицевом счете обсчитываемого плеча)

    if (call->orig) { // на оригинационном плече
        // не тарифицируем звонки, которые переведены на сервисные номера см. поле в базе publiс.server.service_numbers
        // на этих сервисных номерах находятся автоответчики типи "недостаточно денег для звонка" и прочее.
        if (cdr->dst_replace[0] != 0) {
            for (auto srvNumber : callInfo->server->service_numbers) {
                if (srvNumber.compare(cdr->dst_replace) == 0) {
                    call->is_service_number = true;
                    return;
                }
            }
        }

        calcOrigByNumber();                 // Дальше производится тарификация оригинационного плеча
        // при схеме "авторизация по номеру"
    } else {
        calcTermByNumber();                 // Дальше производится тарификация терминационного плеча
        // при схеме "авторизация по номеру"
    }
}

/********************************************************************************************************************
 *   Дальше производится тарификация оригинационного плеча при схеме "авторизация по номеру"
 */

void BillingCall::calcOrigByNumber() {

    if (call->account_version == CALL_ACCOUNT_VERSION_5) {
        // Обсчитываем плечо по пакетной схеме
        if (trace != nullptr) {
            *trace << "INFO|CALL_ACCOUNT_VERSION_5" << "\n";
        }

        calcOrigNNPByNumber();

    } else if (call->account_version == CALL_ACCOUNT_VERSION_4) {
        // Обсчитываем плечо по традиционной схеме
        if (trace != nullptr) {
            *trace << "INFO|CALL_ACCOUNT_VERSION_4" << "\n";
        }

        setupLogTariff(); //  присоединяется действующий LogTariff для вызова на org-плече
        setupMainTariff(); // присоединяется действующий локальный тариф для вызова на org-плече

        setupBilledTime(); //  вычисляется тарифицируемая длительность звонка согласно настройкам
        // (бесплатные секунды, тарификация по мин/по сек) действующего тарифа для оригинационного плеча.

        setupPackagePricelist(); // поисх подходящего пакета для обсчета оригинационного плеча, если находим, используем его

        if (callInfo->servicePackagePricelist == nullptr) { // Для плеча нет подходящего пакета,
            // считаем цену по прайслистам

            setupTariff();  //  Для расчитанного ранее типа звонка (моб не моб, тип зональности)
            // в поле callInfo->tariff присоединяется действующий для этого случая тариф


            setupPricelist(
                    callInfo->tariff->pricelist_id); // присоединяем в callInfo->pricelist прайслист по pricelist_id
            // при расчете с авторизацией по транку прайслист должен быть
            // выбран ранее по коду

            setupPrice(call->dst_number);                   // Расчитываем по ранее выбранному прайслисту и B-номеру
        } else {           // Для звонка найден подходящий пакет
            setupPricelist();                               // присоединяем в callInfo->pricelist прайслист по pricelist_id
            // при расчете с авторизацией по транку прайслист должен быть
            // выбран ранее из пакета.

            setupPrice();                                   //  Расчитываем по ранее выбранному прайслисту и
            //  номера цену звонка на плече
        }

        setupFreemin();  // Учитываем расход предоплаченных минут при локальных звонках

        setupPackagePrepaid();

        setupCost();                                        // Расчет стоимости плеча. С учетом остатка по найденому пакету
    } else {
        throw CalcException("UNKNOW ACCOUNT VERSION");
    }


}

/********************************************************************************************************************
 *   Дальше производится тарификация терминационного плеча при схеме "авторизация по номеру"
 */

void BillingCall::calcTermByNumber() {

    setupLogTariff();   //  присоединяется действующий LogTariff для вызова на терм-плече

    setupMainTariff();  // присоединяется действующий локальный тариф для вызова на терм-плече
    // в поле callInfo->mainTariff

    setupBilledTime();  //  вычисляется тарифицируемая длительность звонка согласно настройкам
    // (бесплатные секунды, тарификация по мин/по сек) действующего тарифа для терм-плеча.

    if (isUsage7800()) {  // если обсчитвается терминационное плечо вызова на номер 7800
        if (trace != nullptr) {
            *trace << "INFO|TARIFFICATION 7800" << "\n";
        }

        setupPricelist(
                callInfo->mainTariff->pricelist_id); // присоединяем в callInfo->pricelist прайслист по pricelist_id
        // при расчете с авторизацией по транку прайслист должен быть
        // выбран ранее по коду

        setupPrice(call->src_number);                       // Расчитываем по ранее выбранному прайслисту и
        // номеру на плече цену звонка на номер 7800 для

        setupCost();                                        // Расчет стоимости плеча. С учетом остатка по найденому пакету

        call->cost = -call->cost;                          // отрабатываем нюанс, что владелец 7800 сам платит за входящие звонки
    }
}

/********************************************************************************************************************
 * на основании значений noa и правил препроцессинга номера для транка (таблица auth.trunk_number_preprocessing)
 * дополняет номера необходимыми префиксами слева, приводя их к 10-значному виду.
 */

void BillingCall::numberPreprocessing() {
    int order = 1;

    bool srcNumberPreprocessingDone = false;
    bool dstNumberPreprocessingDone = false;
    while (true) {
        auto numberPreprocessing = repository->getTrunkNumberPreprocessing(callInfo->trunk->id, order);
        if (numberPreprocessing == nullptr) {
            break;
        }

        if (!srcNumberPreprocessingDone && numberPreprocessing->src && cdr->src_noa == numberPreprocessing->noa &&
            strlen(cdr->src_number) == numberPreprocessing->length) {
            char tmpNumber[20];
            char tmp[20];

            sprintf(tmpNumber, "%lld", call->src_number);
            strcpy(tmp, numberPreprocessing->prefix);
            strcat(tmp, tmpNumber);

            call->src_number = atoll(tmp);

            if (trace != nullptr) {
                *trace << "INFO|NUMBER PREPROCESSING|SET SRC_NUMBER = " << call->src_number << "\n";
            }
            srcNumberPreprocessingDone = true;
        }

        if (!dstNumberPreprocessingDone && !numberPreprocessing->src && cdr->dst_noa == numberPreprocessing->noa &&
            strlen(cdr->dst_number) == numberPreprocessing->length) {
            char tmpNumber[20];
            char tmp[20];

            sprintf(tmpNumber, "%lld", call->dst_number);
            strcpy(tmp, numberPreprocessing->prefix);
            strcat(tmp, tmpNumber);

            call->dst_number = atoll(tmp);

            if (trace != nullptr) {
                *trace << "INFO|NUMBER PREPROCESSING|SET DST_NUMBER = " << call->dst_number << "\n";
            }
            dstNumberPreprocessingDone = true;
        }

        order++;
    }
}

/******************************************************************************************************************
 *  происходит подстановка в поле src_number из поля redirect number при наличии.
 *  В случае наличие на транке флага orig_redirect_number_7800 выполняется подмена dst_номера
 *  на технический на основании таблицы  billing.service_number
 */

void BillingCall::processRedirectNumber() {
    if (callInfo->trunk->orig_redirect_number) {

        long long int redirect_number = atoll(cdr->redirect_number);

        if (redirect_number > 0) {

            processLineWithoutNumber(call, cdr->redirect_number);

            if (trace != nullptr) {
                *trace << "INFO|PROCESS REDIRECT NUMBER|SET SRC_NUMBER = " << call->src_number << "\n";
            }
        }
    } else if (callInfo->trunk->orig_redirect_number_7800) {
        ServiceNumber *serviceNumber = repository->getServiceNumberByTechnicalNumber(getRemoteNumber());
        if (serviceNumber) {

            long long int replacement_number = atoll(serviceNumber->did);

            if (replacement_number > 0) {
                call->dst_number = replacement_number;

                if (trace != nullptr) {
                    *trace << "INFO|PROCESS REPLACEMENT NUMBER 7800|SET DST_NUMBER = " << call->dst_number << "\n";
                }
            }
        }
    }

}

/******************************************************************************************************************
 *  в структуру call рассчитываются поля  geo_mob и geo_id, geo_operator_id
 *  (флаг мобильный, регион и опеатор на другом плече)
 *  (для org плеча - рассчитываем гео-параметры  Б номера, для term плеча - для А- номера) ,
 *        geo_mob  -  флаг, что на противоположном плече находится мобильный номер
 *        geo_id   - подбирается по номеру на противоположном плече (таблица  geo.prefix)
 *        geo_operator_id - подбирается по номеру на противоположном плече (таблица  geo.prefix)
 */

void BillingCall::processGeo() {

    auto mobPrefix = repository->getMobPrefix(call->orig ? call->dst_number : call->src_number);
    if (mobPrefix != nullptr) {
        call->geo_mob = mobPrefix->mob;
        if (trace != nullptr) {
            *trace << "INFO|SET GEO_MOB = " << call->geo_mob << "\n";
        }
    }

    auto geoPrefix = repository->getGeoPrefix(call->orig ? call->dst_number : call->src_number);
    if (geoPrefix != nullptr) {
        call->geo_id = geoPrefix->geo_id;
        call->geo_operator_id = geoPrefix->geo_operator_id;;
        if (trace != nullptr) {
            *trace << "INFO|SET GEO_ID = " << call->geo_id << "\n";
        }
    }

}

/*****************************************************************************************************************
 *  в структуру call рассчитывается поля mob и destination_id
 *  mob - флаг звонка на мобильный номер
 *  destination_id - тип звонка (локальный, внутризоновый, междугородный, международный)
 */

void BillingCall::processDestinations() {

    auto mobPrefix = repository->getMobPrefix(call->dst_number);
    if (mobPrefix != nullptr) {
        call->mob = mobPrefix->mob; // Флаг "Звонок на мобильный"
        if (trace != nullptr) {
            *trace << "INFO|SET MOB = " << call->mob << "\n";
        }
    }

    auto geoPrefix = repository->getGeoPrefix(call->dst_number);
    if (geoPrefix != nullptr) {
        call->destination_id = getDest(geoPrefix->geo_id);
        if (trace != nullptr) {
            *trace << "INFO|SET DEST = " << call->destination_id << "\n";
        }
    }

}

/********************************************************************************************************************
 *  Вычисление типа звонка.  Заполняется поле call->destination_id
 *          -1 - локальный вызов
 *           0 - внутризоновый вызов
 *           1 - национальный (междугородний)
 *           2 - международный
 */


int BillingCall::getDest(int geo_id) {

    callInfo->geo = repository->getGeo(geo_id);
    if (callInfo->geo == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|GEO NOT FOUND" << "\n";
        }
        return 2;
    }

    if (!call->mob && callInfo->instanceSettings->city_id > 0 &&
        callInfo->geo->city_id == callInfo->instanceSettings->city_id) {
        return -1;
    }

    auto regionIds = callInfo->instanceSettings->getRegionIds();
    for (auto it = regionIds.begin(); it != regionIds.end(); ++it) {
        if (callInfo->geo->region_id == *it) {
            return 0;
        }
    }

    if (callInfo->instanceSettings->country_id > 0 &&
        callInfo->geo->country_id == callInfo->instanceSettings->country_id) {
        return 1;
    }

    return 2;
}

/****************************************************************************************************************
 *  Проверяем поле src_number на случай звонка с линии без номера.
 *  при таких звонказ в поле номера содержится что-то такое - 74996850009=2A4026
 *  функция отрезазает префикс и оставляет в поле внутренний номер такой линии - здесь будет 4026
 */

void BillingCall::processLineWithoutNumber(Call *call, char *cdr_num) {
    char *pos = nullptr;
    if (pos == nullptr) {
        pos = strstr(cdr_num, "=2A");
        if (pos != nullptr) pos += 3;
    }
    if (pos == nullptr) {
        pos = strstr(cdr_num, "=2B");
        if (pos != nullptr) pos += 3;
    }
    if (pos == nullptr) {
        pos = strstr(cdr_num, "*");
        if (pos != nullptr) pos += 1;
    }
    if (pos == nullptr) {
        pos = strstr(cdr_num, "+");
        if (pos != nullptr) pos += 1;
    }
    if (pos != nullptr) {
        call->src_number = atoll(pos);
        if (trace != nullptr) {
            *trace << "INFO|LINE WITHOUT NUMBER|SET SRC_NUMBER = " << call->src_number << "\n";
        }
    } else {
        if (cdr_num != nullptr) call->src_number = atoll(cdr_num);
    }

}


/************************************************************************************************************
 *  Вычисление номера на обсчитываемом плеча вызова
 */

long long int BillingCall::getNumber() {
    return call->orig ? call->src_number : call->dst_number;
}

/************************************************************************************************************
 *  Вычисление номера на противоположном плече вызова
 */

long long int BillingCall::getRemoteNumber() {
    return call->orig ? call->dst_number : call->src_number;
}

/************************************************************************************************************
 *  Вычисление названия транка обсчитываемого плеча вызова
 */

char *BillingCall::getRoute() {
    return call->orig ? cdr->src_route : cdr->dst_route;
}

/************************************************************************************************************
 *  Вычисление названия противоположного транка для обсчитываемого плеча вызова
 */

char *BillingCall::getRemoteRoute() {
    return call->orig ? cdr->dst_route : cdr->src_route;
}

int BillingCall::getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes) {

    if (freeFirstMinutes && len <= app().conf.billing_free_seconds)
        return 0;

    if (len <= 0)
        return 0;

    if (byMinutes) {

        int s = len % 60;
        if (s == 0)
            return len;

        return len + 60 - s;

    } else {

        if (fullFirstMinute && len < 60)
            return 60;

        return len;

    }
}

/*******************************************************************************************************************
 *   Проверка - на расчитываемом плече номер 7800 ? да/нет
 */

bool BillingCall::isUsage7800() {
    char tmpNumber[20];
    sprintf(tmpNumber, "%lld", getNumber());

    return tmpNumber[0] == '7' && tmpNumber[1] == '8' && tmpNumber[2] == '0' && tmpNumber[3] == '0';
}

/******************************************************************************************************************
 *  вычисляются и присоединяются к callInfo->trunk параметры транка на плече   Таблица - auth.trunk
 *  вычисляется  флаг - наш / не наш транк
 */

void BillingCall::setupTrunk() {
    callInfo->trunk = repository->getTrunkByName(getRoute());
    call->server_id = app().conf.instance_id;
    if (callInfo->trunk == nullptr) {
        throw CalcException("TRUNK WAS NOT FOUND");
    }

    call->trunk_id = callInfo->trunk->id;  // Из таблицы - auth.trunk
    call->our = callInfo->trunk->our_trunk || callInfo->trunk->auth_by_number;

    callInfo->server = repository->getServer(callInfo->trunk->server_id);
    if (callInfo->server == nullptr) {
        throw CalcException("SERVER WAS NOT FOUND");
    }

    call->server_id = callInfo->server->id;

    callInfo->instanceSettings = repository->getInstanceSettings(callInfo->trunk->server_id);
    if (callInfo->instanceSettings == nullptr) {
        throw CalcException("INSTANCESETTINGS WAS NOT FOUND");
    }

    if (trace != nullptr) {
        *trace << "INFO|REGION_ID|" << callInfo->server->id << "\n";
    }

}

/******************************************************************************************************************
 *  При расчете по транковой авторизации необходимо выбрать минимальную цену на оригинационном плече
 *
 */

void BillingCall::setupEffectiveOrigTrunkSettings() {
    vector<ServiceTrunkOrder> trunkSettingsOrderList;
    set<int> nnpDestinationIds;

    // Вычисляем все nnp-направления для B-номера звонка
    NNPNumberRange *term_nnpNumberRange = repository->getNNPNumberRange(call->dst_number, trace);

    repository->getNNPDestinationByNumberRange(nnpDestinationIds, term_nnpNumberRange, trace);

    repository->getTrunkSettingsOrderList(trunkSettingsOrderList, callInfo->trunk, call->src_number, call->dst_number,
                                          nnpDestinationIds, SERVICE_TRUNK_SETTINGS_ORIGINATION);
    // получили список возможных прайсов на этом транке для обсчитываемой пары АБ.

    repository->orderOrigTrunkSettingsOrderList(trunkSettingsOrderList);
    // отсортировали по цене

    if (trunkSettingsOrderList.size() > 0) {
        auto order = trunkSettingsOrderList.at(0);
        callInfo->account = order.account;
        callInfo->serviceTrunk = order.serviceTrunk;
        callInfo->trunkSettings = order.trunkSettings;
        callInfo->pricelist = order.pricelist;
        callInfo->price = order.price;

        if (callInfo->account != nullptr && callInfo->account->account_version == CALL_ACCOUNT_VERSION_5) {
            callInfo->nnpPackage = order.nnpPackage;
            callInfo->nnpPackagePricelist = order.nnpPackagePricelist;
            callInfo->nnpPackagePrice = order.nnpPackagePrice;
            call->nnp_package_price_id = order.nnpPackagePrice->id;
            call->nnp_package_pricelist_id = order.nnpPackagePricelist->id;
            call->nnp_package_id = order.nnpPackage->id;
            call->rate = order.nnp_price;
        }

    }
}

/******************************************************************************************************************
 *  При расчете по транковой авторизации необходимо выбрать минимальную цену на терминационном плече
 *
 */

void BillingCall::setupEffectiveTermTrunkSettings() {
    vector<ServiceTrunkOrder> trunkSettingsOrderList;
    set<int> nnpDestinationIds;

    repository->getNNPDestinationByNumberRange(nnpDestinationIds, callInfo->nnpNumberRange, trace);

    repository->getTrunkSettingsOrderList(trunkSettingsOrderList, callInfo->trunk, call->src_number, call->dst_number,
                                          nnpDestinationIds, SERVICE_TRUNK_SETTINGS_TERMINATION);

    Trunk *orig_trunk = repository->getTrunkByName(cdr->src_route);
    if (orig_trunk == nullptr) {
        throw CalcException("ORIG TRUNK WAS NOT FOUND");
    }

    // отсортировали по цене, причем применили настройку на оригинационном плече - использовать или нет минималки на терм-плече

    repository->orderTermTrunkSettingsOrderList(trunkSettingsOrderList, orig_trunk->sw_minimalki, call->connect_time);

    if (trunkSettingsOrderList.size() > 0) {
        auto order = trunkSettingsOrderList.at(0);
        callInfo->account = order.account;
        callInfo->serviceTrunk = order.serviceTrunk;
        callInfo->trunkSettings = order.trunkSettings;
        callInfo->pricelist = order.pricelist;
        callInfo->price = order.price;
        // сохраняем в структуре callInfo самый дешевый прайслист и цену для этой пары АБ.
        call->trunk_settings_stats_id = order.statsTrunkSettings->id;

        if (callInfo->account != nullptr && callInfo->account->account_version == CALL_ACCOUNT_VERSION_5) {
            callInfo->nnpPackage = order.nnpPackage;
            callInfo->nnpPackagePricelist = order.nnpPackagePricelist;
            callInfo->nnpPackagePrice = order.nnpPackagePrice;
            call->nnp_package_price_id = order.nnpPackagePrice->id;
            call->nnp_package_pricelist_id = order.nnpPackagePricelist->id;
            call->nnp_package_id = order.nnpPackage->id;
            call->rate = order.nnp_price;
        }

    }
}

/******************************************************************************************************************
 *  Присоединение данных по транку на плече из структуры callInfo в call
 */

void BillingCall::setupServiceTrunk() {
    if (callInfo->serviceTrunk == nullptr) {
        throw CalcException("SERVICE TRUNK WAS NOT FOUND");
    }
    call->trunk_service_id = callInfo->serviceTrunk->id;
}

/******************************************************************************************************************
 * к структуре callinfo присоединяется структура ServiceNumber для обсчитываемого плеча
 * ( инфомация об услуге номер, )
 */

void BillingCall::setupServiceNumber() {
    callInfo->serviceNumber = repository->getServiceNumber(getNumber());
    if (callInfo->serviceNumber == nullptr) {
        throw CalcException("SERVICE NUMBER WAS NOT FOUND");
    }
    call->number_service_id = callInfo->serviceNumber->id;
}

/*******************************************************************************************************************
 *  Присоединяем структуру callInfo->serviceTrunk (Услуга Транк) по транку плеча
 *
 *
 */


void BillingCall::setupServiceTrunkForNumber() {
    callInfo->serviceTrunk = repository->getServiceTrunk(callInfo->trunk->id);

    if (!callInfo->trunk->our_trunk) {
        if (callInfo->serviceTrunk == nullptr) {
            throw CalcException("SERVICE TRUNK WAS NOT FOUND");
        }

        if (callInfo->serviceNumber->client_account_id != callInfo->serviceTrunk->client_account_id) {
            throw CalcException("SERVICE NUMBER ACCOUNT DIFFERENT FROM SERVICE TRUNK_ACCOUNT");
        }
    }

    if (callInfo->serviceTrunk != nullptr) {
        call->trunk_service_id = callInfo->serviceTrunk->id;
    }
}

/********************************************************************************************************************
 *  Присоединяем к callInfo->account информацию о лицевом счете расчитываемого плеча звонка
 *
 */

void BillingCall::setupAccount() {
    if (callInfo->serviceNumber != nullptr) {
        callInfo->account = repository->getAccount(callInfo->serviceNumber->client_account_id);
    } else if (callInfo->serviceTrunk != nullptr) {
        callInfo->account = repository->getAccount(callInfo->serviceTrunk->client_account_id);
    }

    if (callInfo->account == nullptr) {
        throw CalcException("ACCOUNT WAS NOT FOUND");
    }

    callInfo->make_dt(); // учитываем часовой пояс регионального сервера

    call->account_id = callInfo->account->id;
    call->account_version = callInfo->account->account_version;
}

/********************************************************************************************************************
 *  присоединяем в callInfo->pricelist прайслист по pricelist_id
 */

void BillingCall::setupPricelist(int pricelist_id) {
    if (pricelist_id != 0) {
        callInfo->pricelist = repository->getPricelist(pricelist_id);
    }
    if (callInfo->pricelist == nullptr) {
        throw CalcException("PRICELIST NOT FOUND");
    }

    if (trace != nullptr) {
        *trace << "INTO|PRICELIST|";
        callInfo->pricelist->dump(*trace);
        *trace << "\n";
    }

    call->pricelist_id = callInfo->pricelist->id;
}

/********************************************************************************************************************
*  Расчитываем по ранее выбранному прайслисту и префиксу номера цену звонка на плече
*/

void BillingCall::setupPrice(long long int numberPrefix) {
    if (numberPrefix != 0) {
        callInfo->price = repository->getPrice(callInfo->pricelist->id, numberPrefix);
    }
    if (callInfo->price == nullptr) {
        throw CalcException("PRICE NOT FOUND");
    }

    if (trace != nullptr) {
        *trace << "INTO|PRICELIST|";
        callInfo->pricelist->dump(*trace);
        *trace << "\n";
    }

    call->prefix = atoll(callInfo->price->prefix);
    call->rate = callInfo->price->price;
}

/******************************************************************************************************************
 *   вычисляется тарифицируемая длительность звонка согласно настройкам
 *   (бесплатные секунды, тарификация по мин/по сек) действующего тарифа для расчитываемого плеча.
 */

void BillingCall::setupBilledTime() {
    if (callInfo->serviceNumber) {
        call->billed_time = getCallLength(
                cdr->session_time,
                callInfo->mainTariff->tariffication_by_minutes,
                callInfo->mainTariff->tariffication_full_first_minute,
                callInfo->mainTariff->tariffication_free_first_seconds
        );
    } else {
        call->billed_time = getCallLength(
                cdr->session_time,
                callInfo->pricelist->tariffication_by_minutes,
                callInfo->pricelist->tariffication_full_first_minute,
                false
        );
    }
}

/****************************************************************************************************************
 *   Расчет стоимости плеча. С учетом остатка по найденому пакету
 *   и знака (зарабатывает или тратит клиент на этом плече)
 *   */

void BillingCall::setupCost() {
    int paid_time = call->billed_time - call->package_time;

    if (paid_time > 0) {
        call->cost = paid_time * call->rate / 60.0;
        call->cost = call->orig ? -call->cost : call->cost;
    } else {
        call->cost = 0;
    }

    if (call->billed_time > 0) {
        call->package_credit = call->package_time * call->rate / 60.0;
        call->package_credit = call->orig ? -call->package_credit : call->package_credit;
    } else {
        call->package_credit = 0;
    }
}

/*******************************************************************************************************************
 *   Для Услуги Номер выбирается действующий LogTariff (таблица billing.tariff_change_log )
 *   каждая запись содержит 6 тарифов по зонам и флагу моб.
 */

void BillingCall::setupLogTariff() {
    callInfo->logTariff = repository->getTariffLog(callInfo->serviceNumber->id);
    if (callInfo->logTariff == nullptr) {
        throw CalcException("TARIFF CHANGE LOG NOT FOUND");
    }
}

/*******************************************************************************************************************
 *   В callInfo->mainTariff присоединяется действующий основной локальный тариф
 */

void BillingCall::setupMainTariff() {
    callInfo->mainTariff = repository->getTariff(callInfo->logTariff->tariff_id_local);
    if (callInfo->mainTariff == nullptr) {
        throw CalcException("TARIFF NOT FOUND");
    }
}

/******************************************************************************************************************
 *  Для расчитанного ранее типа звонка (моб не моб, тип зональности) в поле callInfo->tariff присоединяется
 *  действующий для этого случая тариф
 */

void BillingCall::setupTariff() {
    int tariffId = 0;
    if (call->isLocal())
        tariffId = callInfo->logTariff->tariff_id_local;
    else if (call->isZonaMob())
        tariffId = callInfo->logTariff->tariff_id_local_mob;
    else if (call->isZonaStd())
        tariffId = callInfo->logTariff->tariff_id_russia;
    else if (call->isRussianStd())
        tariffId = callInfo->logTariff->tariff_id_russia;
    else if (call->isRussianMob())
        tariffId = callInfo->logTariff->tariff_id_russia_mob;
    else if (call->isInternational())
        tariffId = callInfo->logTariff->tariff_id_intern;

    callInfo->tariff = repository->getTariff(tariffId);
    if (callInfo->tariff == nullptr) {
        throw CalcException("TARIFF NOT FOUND");
    }
}


/******************************************************************************************************************
 *   Поиск подходящего пакета для того, что бы тарифицировать им звонок.
 *
 *   В базе данных по пакетам:
 *      Billing.service_package - Подключение пакета из Billing.tariff_package к транкам или номера на время
 *      Billing.tariff_package  - пакеты состоят из (направления, предоплачнных минут и прайслиста)
 *   В результате заполняет         callInfo->pricelist, callInfo->price, callInfo->servicePackagePricelist,
 *                            call->service_package_id - параметрами примененного пакета.
  */

void BillingCall::setupPackagePricelist() {
    vector<ServicePackage *> packages;

    if (callInfo->serviceNumber != nullptr) {
        repository->getAllServiceNumberPackage(packages, callInfo->serviceNumber->id);
    } else if (callInfo->serviceTrunk != nullptr) {
        repository->getAllServiceTrunkPackage(packages, callInfo->serviceTrunk->id);
    } else {
        return;
    }

    Pricelist *pricelist;
    PricelistPrice *price;

    for (auto package : packages) {
        auto tariff = repository->getTariffPackage(package->tariff_package_id);
        if (tariff == nullptr) {
            continue;
        }

        if (tariff->pricelist_id == 0) {
            continue;
        }

        if (!matchTariffPackageDestination(tariff)) {
            continue;
        }

        pricelist = repository->getPricelist(tariff->pricelist_id);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE PACKAGE SKIPPED|CAUSE PRICELIST NOT FOUND" << "\n";
            }
            continue;
        }

        price = repository->getPrice(pricelist->id, call->dst_number);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE PACKAGE SKIPPED|CAUSE PRICELIST PRICE NOT FOUND" << "\n";
            }
            continue;
        }

        callInfo->pricelist = pricelist;
        callInfo->price = price;
        callInfo->servicePackagePricelist = package;
        call->service_package_id = package->id;
    }
}

/********************************************************************************************************************
 *  Учитываем использование предоплаченных минут на локальном тарифе.
 *
 *  Вычисленное значение сохраняем в call->package_time
 */

void BillingCall::setupFreemin() {
    if (call->billed_time == 0) {
        return;
    }

    if (!call->isLocal()) {
        return;
    }

    // Только при локальных звонках

    int tariffFreeSeconds = 60 * callInfo->mainTariff->freemin *
                            (callInfo->mainTariff->freemin_for_number ? 1 : callInfo->serviceNumber->lines_count);
    if (trace != nullptr) {
        *trace << "INFO|FREEMIN|TARIFF " << tariffFreeSeconds << " SECONDS, TARIFF_ID: " << callInfo->mainTariff->id <<
               "\n";
    }

    if (tariffFreeSeconds <= 0) {
        return;
    }

    StatsFreemin *stats = repository->billingData->statsFreeminGetCurrent(callInfo);

    if (trace != nullptr) {
        *trace << "INFO|FREEMIN|USED " << stats->used_seconds << " SECONDS, STAT_ID: " << stats->id << "\n";
    }

    int availableFreeSeconds = tariffFreeSeconds - stats->used_seconds;
    if (availableFreeSeconds < 0) {
        availableFreeSeconds = 0;
    }
    if (availableFreeSeconds > call->billed_time) {
        availableFreeSeconds = call->billed_time;
    }
    if (availableFreeSeconds > 0) {
        if (trace != nullptr) {
            *trace << "INFO|FREEMIN|APPLY " << availableFreeSeconds << " SECONDS, STAT_ID: " << stats->id << "\n";
        }

        call->service_package_id = 1; //  Это значит, что минуты выбираются из предоплаченных локального тарифа

        call->service_package_stats_id = stats->id; // запоминаем применный к вызову пакет из billing.stats_packege
        call->package_time = availableFreeSeconds;
    }

}

/******************************************************************************************************************
 *   Учитываем, сколько секунд из тарифицированного вызова можно учесть из подключенных пакетов
 *
 *  */

void BillingCall::setupPackagePrepaid() {
    if (call->billed_time == 0 || call->service_package_id > 0) {
        return;
    }

    vector<ServicePackage *> packages;

    // Загружаем все возможные пакеты на услугах Номер и Транк этого плеча вызова, пробуем их учесть.

    if (callInfo->serviceNumber != nullptr) {
        repository->getAllServiceNumberPackage(packages, callInfo->serviceNumber->id);
    } else if (callInfo->serviceTrunk != nullptr) {
        repository->getAllServiceTrunkPackage(packages, callInfo->serviceTrunk->id);
    } else {
        return;
    }

    int packageSeconds = 0;
    ServicePackage *servicePackage = nullptr;
    TariffPackage *tariffPackage = nullptr;
    StatsPackage *statsPackage = nullptr;


    for (auto package : packages) {
        auto tariff = repository->getTariffPackage(package->tariff_package_id);
        if (tariff == nullptr) {
            continue;
        }

        if (tariff->prepaid_minutes == 0) {
            continue;
        }

        if (!matchTariffPackageDestination(tariff)) { // Проверка, можно ли применить Тариф к вызову
            continue;
        }

        // Можно

        StatsPackage *stats = repository->billingData->statsPackageGetCurrent(callInfo, package, tariff);

        if (trace != nullptr) {
            *trace << "DEBUG|STATS PACKAGE|";
            stats->dump(*trace);
            *trace << "\n";
        }

        int availableSeconds = stats->paid_seconds - stats->used_seconds;
        if (availableSeconds < 0) {
            availableSeconds = 0;
        }

        if (availableSeconds >= call->billed_time) {
            packageSeconds = call->billed_time;
            servicePackage = package;
            tariffPackage = tariff;
            statsPackage = stats;
            break;
        } else {
            if (availableSeconds > packageSeconds) {
                packageSeconds = availableSeconds;
                servicePackage = package;
                tariffPackage = tariff;
                statsPackage = stats;
            }
        }
    }

    if (servicePackage != nullptr) {
        call->package_time = packageSeconds;
        call->service_package_id = servicePackage->id;
        call->service_package_stats_id = statsPackage->id;
        callInfo->servicePackagePrepaid = servicePackage;
        callInfo->tariffPackagePrepaid = tariffPackage;
    }
}

/*********************************************************************************************************************
 *   billing.stat_destination_prefixlists - таблица связи направлений и префиксов
 *   billing.stat_prefixlist - префикслисты для направлений (типы смотри в models/ServiceTrunk.h )
 *
 *   Проверка - попадает ли номер call->dst_number в *tariff
 *
 */


bool BillingCall::matchTariffPackageDestination(TariffPackage *tariff) {
    vector<int> prefixlistIds;  // тут будет список префикс-листов
    repository->getAllStatPrefixlistIdsByStatDestinationId(prefixlistIds, tariff->destination_id);
    // Вычисляем список все префикс-листов по направлению из тарифа

    for (int prefixlistId : prefixlistIds) {
        StatPrefixlist *prefixlist = repository->getStatPrefixlist(prefixlistId);
        //  выбрали префикслиста из списка, дальше работаем с ним таблица
        if (prefixlist != nullptr) {

            if (prefixlist->type_id == STAT_PREFIXLIST_TYPE_MANUAL) {

                for (long long int prefix : prefixlist->prefixes) {
                    // Пытаемся найти подходящий префикс из списка в поле stat_prefixlist.prefixes
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|TRY MATCH NUMBER " << call->dst_number << " BY PREFIX " << prefix << "\n";
                    }

                    char tmpPrefix[20];
                    sprintf(tmpPrefix, "%lld", call->dst_number);

                    size_t len = strlen(tmpPrefix);
                    while (len > 0) {
                        tmpPrefix[len] = 0;
                        if (prefix == atoll(tmpPrefix)) {
                            if (trace != nullptr) {
                                *trace << "INFO|MATCH|OK. PREFIX " << prefix << "\n";
                            }
                            return true; // подходящий префикс с типом Мануал в поле  stat_prefixlist.prefixes
                            // нашелся
                        }
                        len -= 1;
                    }
                }

            } else if (prefixlist->type_id == STAT_PREFIXLIST_TYPE_ROSLINK) { // тип префикслиста - РОСЛИНК
                if (trace != nullptr) {
                    *trace << "INFO|MATCH|TRY MATCH BY GEO\n";
                    *trace << "|STAT PREFIXLIST|";
                    prefixlist->dump(*trace);
                    *trace << "\n";
                    *trace << "|GEO|";
                    callInfo->geo->dump(*trace);
                    *trace << "\n";
                }

                if (prefixlist->sub_type == STAT_PREFIXLIST_SUBTYPE_FIXED && call->mob) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT FIXED\n";
                    }
                    continue;
                }

                if (prefixlist->sub_type == STAT_PREFIXLIST_SUBTYPE_MOBILE && !call->mob) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MOBILE\n";
                    }
                    continue;
                }

                if (prefixlist->country_id > 0 && callInfo->geo != nullptr &&
                    callInfo->geo->country_id != prefixlist->country_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH COUNTRY\n";
                    }
                    continue;
                }

                if (prefixlist->region_id > 0 && callInfo->geo != nullptr &&
                    callInfo->geo->region_id != prefixlist->region_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH REGION\n";
                    }
                    continue;
                }

                if (prefixlist->city_id > 0 && callInfo->geo != nullptr &&
                    callInfo->geo->city_id != prefixlist->city_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH CITY\n";
                    }
                    continue;
                }

                if (prefixlist->exclude_operators && prefixlist->operators.size() > 0) {
                    bool exclude = false;
                    for (int excludeOperatorId : prefixlist->operators) {
                        if (call->geo_operator_id == excludeOperatorId) {
                            exclude = true;
                            break;
                        }
                    }
                    if (exclude) {
                        if (trace != nullptr) {
                            *trace << "INFO|MATCH|EXIT. NOT MATCH EXCULDE OPERATORS\n";
                        }
                        continue;
                    }
                }

                if (!prefixlist->exclude_operators && prefixlist->operators.size() > 0) {
                    bool exclude = true;
                    for (int includeOperatorId : prefixlist->operators) {
                        if (call->geo_operator_id == includeOperatorId) {
                            exclude = false;
                            break;
                        }
                    }
                    if (exclude) {
                        if (trace != nullptr) {
                            *trace << "INFO|MATCH|EXIT. NOT MATCH INCLUDE OPERATORS\n";
                        }
                        continue;
                    }
                }

                if (trace != nullptr) {
                    *trace << "INFO|MATCH|OK. BY GEO\n";
                }
                return true;
            }

        }
    }
    return false;
}

bool BillingCall::processSignalingCallId() {

    if (call->orig && callInfo->trunk != nullptr && callInfo->trunk->our_trunk) {
        call->signalling_call_id = cdr->in_sig_call_id;
    }

    if (!call->orig && callInfo->trunk != nullptr && callInfo->trunk->our_trunk) {
        call->signalling_call_id = cdr->out_sig_call_id;
    }

}