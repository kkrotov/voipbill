//#include "BaseTest.h"
#include <string.h>
#include <cxxtest/TestSuite.h>
#include "../../src-bill/common.h"
#include "../../src-bill/common.cpp"
#include "../../src-bill/classes/Log.cpp"

/**
 * Класс проверяющий корректную работу функции removeHtmlTags.
 *
 * Проверка делается с помощью сравнения реальных результатов
 *  работы функции и ожиданных.
 *
 */
class RemoveHtmlTagsTest : public CxxTest::TestSuite {

    public:

    void testThis (void) {

       TS_TRACE ("Running RemoveHtmlTags test function");
       const int TEST_NUM = 4;
       string strToTest[TEST_NUM] = {"",
                                     "Te<s  <s > >sted",
                                     "Te<s >>s<s >ted",
                                     "Te<<s<s >>sted"};
       bool returnToTest[TEST_NUM];

       string checkStr[TEST_NUM] = {"",
                                    "Tested",
                                    "Te>sted",
                                    "Te<sted"};
       bool checkReturn[TEST_NUM] = {false,
                                     true,
                                     true,
                                     true};


       for (int i = 0; i < TEST_NUM; i++)
           returnToTest[i] = removeHtmlTags (strToTest[i]);

       for (int i = 0; i < TEST_NUM; i++) {
           TS_ASSERT (!strcmp(strToTest[i].c_str(), checkStr[i].c_str()))
           TS_ASSERT (checkReturn[i] == returnToTest[i]);
       }
           
    }
};
