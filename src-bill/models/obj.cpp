#include "obj.h"
#include "../classes/AppBill.h"

void _CallObj::make_dt() {
    parseDateTime((char*) &time, dt);
}

bool _CallObj::isLocal() {
    return dest < 0;
}

bool _CallObj::isLocalOrZona() {
    return dest <= 0;
}

bool _CallObj::isZona() {
    return dest == 0;
}

bool _CallObj::isZonaMob() {
    return dest == 0 && mob == true;
}

bool _CallObj::isZonaStd() {
    return dest == 0 && mob == false;
}

bool _CallObj::isRussian() {
    return dest == 1;
}

bool _CallObj::isRussianMob() {
    return dest == 1 && mob == true;
}

bool _CallObj::isRussianStd() {
    return dest == 1 && mob == false;
}

bool _CallObj::isInternational() {
    return dest == 2;
}

bool _CallObj::isSNG() {
    return dest == 3;
}

bool _CallObj::isTrank() {
    return instance_id >= 100;
}

bool _CallObj::isCallFromAnotherInstance() {
    return instance_id >= 80
            && instance_id < 100
            && instance_id != app().conf.instance_id;
}
