#pragma once

#include "../classes/Repository.h"

class Price {
public:
    bool   present;
    double price;
    double currency_rate = 1;
    string currency;

    Price() {
        present = false;
        price = 0;
        currency_rate = 1;
        currency = "RUB";
    }

    void set(double price,char *currency) {
        this->price    = price;
        this->currency = currency;
        this->present  = true;
    }

    bool isPresent(){
        return this->present;
    }

    bool operator< (Price &price) {

       if(this->isPresent() && !price.isPresent()) return true;

       if(!this->isPresent() && price.isPresent()) return false;

       if(!this->isPresent() && !price.isPresent()) return false;

       if(this->currency == price.currency) {
           return this->price < price.price;
       } else {
           return (this->price * this->currency_rate) < ( price.price * price.currency_rate);
       }

    }
};

