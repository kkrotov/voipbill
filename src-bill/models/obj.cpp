#include "obj.h"
#include "../classes/AppBill.h"

void _CdrObj::make_dt() {
    parseDateTime((char*) &connect_time, dt);
}

void _CallObj::make_dt() {
    parseDateTime((char*) &time, dt);
}

void _CallObj::initByCdr(CdrObj * cdr) {
    strcpy(cdr_id, cdr->id);
    strcpy(call_time, cdr->connect_time);
    strcpy(src_number, cdr->src_number);
    strcpy(dst_number, cdr->dst_number);
    strcpy(redirect_number, cdr->redirect_number);
}

void _CallObj::initOrigByCdr(CdrObj * cdr) {
    initByCdr(cdr);

    strcpy(call_origin, "orig");
}

void _CallObj::initTermByCdr(CdrObj * cdr) {
    initByCdr(cdr);

    strcpy(call_origin, "term");
}
