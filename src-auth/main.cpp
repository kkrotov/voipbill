#include "classes/AppAuth.h"

int main(int argc, char* argv[]) {

    if (!app().init(argc, argv))
        return 1;

    app().run();


    return 0;

}

