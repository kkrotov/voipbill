#include "classes/AppBill.h"

#include <stdio.h>

int main(int argc, char* argv[]) {

    if (!app().init(argc, argv))
        return 1;

    app().run();

    return 0;

}

