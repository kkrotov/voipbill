#include "classes/App.h"


App app;

int main_test(int argc, char* argv[]);

int main(int argc, char* argv[]) {

    if (!app.init(argc, argv))
        return 1;


    if (app.conf.test_mode)
        return main_test(argc, argv);


    app.run();


    return 0;

}

