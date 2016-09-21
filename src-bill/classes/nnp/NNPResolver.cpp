#include "NNPResolver.h"

NNPResolver::NNPResolver(Repository *repository) {
    this->repository = repository;
    this->trace = nullptr;
}

void NNPResolver::setTrace(stringstream *trace) {
    this->trace = trace;
    repository->trace = trace;
}

void NNPResolver::clearTrace() {
    this->trace = nullptr;
    repository->trace = nullptr;
}

/*********************************************************************************************************************
 *  Поиск кода страны по ключу в базе, пока такого справочника нет, стоит заглушка для кода 643 - это Россия 7
 *
 */

long long int NNPResolver::getCountryCodeByID(int country_code) {

    if (country_code == 643) return 7;

    throw CalcException("COUNTRY_CODE NOT FOUND");

}

/*********************************************************************************************************************
 *  Быстрый поиск number_range в которые входит префикс numberPrefix
 *
 *
 */

void NNPResolver::findAllNumberRange(vector<int>&nnp_number_rangeIds, long long int numberPrefix){

}

/*********************************************************************************************************************
 *   nnp.destination - таблица nnp-направлений
 *   nnp.prefix_destination - связь наполнение направлений именованными наборами префиксов.
 *                            !!! Поле is_addition - набор префикс входит с + или исключается с -
 *   nnp.number_range_prefix - связь набора префиксов с набором диапазонов номеров.
 *   nnp,number_range - диапазоны номеров
 *   Проверка - попадает ли номер numberPrefix в направление nnp_destination_id
 *
 */

bool NNPResolver::matchNumberNNPDestination(long long int numberPrefix, int nnp_destination_id) {

    NNPDestination *nnpDestination = repository->getNNPDestination(nnp_destination_id);
    if (nnpDestination == nullptr)
        throw CalcException("NNPDestination NOT FOUND");



    //string number_from = itoa (nnpDestination->);



    return false;
}

/*********************************************************************************************************************
 *  Проверка - входит ли префикс numberPrefix  в диапазон описанный указателем
 *  на структуру NNPNumberRange *nnpNumberRange
 *
 */

bool NNPResolver::isExistNumberPrefixInNNPNumberRange(long long int numberPrefix, NNPNumberRange *nnpNumberRange) {

    NNPNumberRange

//    --itoa()

    return false;
}