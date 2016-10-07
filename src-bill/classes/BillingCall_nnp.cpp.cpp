#include "BillingCall.h"
#include "../classes/CalcException.h"


/********************************************************************************************************************
 *  Рачсчитываем nnp- параметры для пдеча вызова.
 */


void BillingCall::processNNP() {
    NNPNumberRange *nnpNumberRange;

    if (call->orig) nnpNumberRange = repository->getNNPNumberRange(call->src_number, trace);
    else nnpNumberRange = repository->getNNPNumberRange(call->dst_number, trace);

    if (nnpNumberRange != nullptr) {
        this->callInfo->nnpNumberRange = nnpNumberRange;
        this->call->nnp_operator_id = nnpNumberRange->nnp_operator_id;
        this->call->nnp_region_id = nnpNumberRange->nnp_region_id;
        this->call->nnp_city_id = nnpNumberRange->nnp_city_id;
        this->call->nnp_country_prefix = nnpNumberRange->country_prefix;
        this->call->nnp_ndc = nnpNumberRange->ndc;
        this->call->nnp_is_mob = nnpNumberRange->is_mob;
    }
}