#include "RemoveHtmlTagsTest.h"
#include "../../src-bill/common.h"

RemoveHtmlTagsTest :: RemoveHtmlTagsTest() {
}

/**
 * Записывает результат.
 *
 * Алгоритм:
 * Берет строку из массива strToTest с нужным индексом
 * Вызывает removeHtmlTags для неё.
 * Полученное значение записывает в resultStr.
 * Значение return'а removeHtmlTags записывает в resultReturn.
 *
 * В случае ошибки, записывает текст ошибки в lastError
 * и возвращает false.
 *
 * @param index - номер результата для проверки.
 * @return true запись результата успешна, false запись результата неуспешна
 */
bool RemoveHtmlTagsTest :: setResult   (unsigned int index) {

    if ( index > this->getMaxIndex() ) {
        this->lastError  = "Cant set result:";
        this->lastError += to_string(index);
        this->lastError += " | because MaxIndex:";
        this->lastError += to_string(this->getMaxIndex());

        return false;
    }

    string strToTest[this->maxIndex + 1] = {"",
                                            "Te<s  <s > >sted",
                                            "Te<s >>s<s >ted",
                                            "Te<<s<s >>sted"};

    this->resultReturn = removeHtmlTags (strToTest[index]);
    this->resultStr    = strToTest[index];

    return true;
}

/**
 * Проверяет записанный результат.
 *
 * Алгоритм:
 * Сравнивает this->checkStr и строку из массива checkStr с нужным индексом.
 * Сравнивает this->checkReturn и значение из массива checkReturn с нужным индексом
 *
 * В случае ошибки, записывает текст ошибки в lastError
 * и возвращает true.
 *
 * @param index
 * @return true проверка результата успещна, false проверка результата неуспешна.
 */
bool RemoveHtmlTagsTest :: checkResult (unsigned int index) {

     if ( index > this->getMaxIndex() ) {
        this->lastError  = "Cant check result:";
        this->lastError += to_string(index);
        this->lastError += " | because MaxIndex:";
        this->lastError += to_string(this->getMaxIndex());

        return false;
    }

    string checkStr[this->maxIndex + 1] = {"",
                                           "Tested",
                                           "Te>sted",
                                           "Te<sted"};

    bool checkReturn[this->maxIndex + 1] = {false,
                                            true,
                                            true,
                                            true};

    if (checkStr[index].compare(this->resultStr)) {
        this->lastError  = "Str doesnt match\n";
        this->lastError += "Expected:'";
        this->lastError += checkStr[index];
        this->lastError += "'\n";
        this->lastError += "Received:'";
        this->lastError += this->resultStr;
        this->lastError += "'";

        return false;
    }
    if (checkReturn[index] != this->resultReturn) {
        this->lastError  = "Return doesnt match\n";
        this->lastError += "Expected:'";
        this->lastError += to_string(checkReturn[index]);
        this->lastError += "'\n";
        this->lastError += "Received:'";
        this->lastError += to_string(this->resultReturn);
        this->lastError += "'";

        return false;

    }

    return true;
}

unsigned int RemoveHtmlTagsTest :: getMaxIndex  () const {
    return this->maxIndex;
}

const char* RemoveHtmlTagsTest :: getTestName  () const {
    static const char* name = "Method removeHtmlTags test";
    return name;
}
