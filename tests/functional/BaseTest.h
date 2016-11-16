#pragma once

#include <string>

using namespace std;

/**
 * Базовый класс для unit тестов.
 *
 * Идея тестирования такова:
 * Тест каждого модуля использует класс отнаследованный от BaseTest.
 * Работа тестируемого модуля имеет некий результат.
 * Результаты могут быть разными, например, при разных аргументах.
 *  поэтому результаты имеют индекс.
 * Тестирование заключается в сравнении полученного результата с ожидаемым.
 * При возникновении несовпадения текст ошибки должен быть записан в lastError.
 * Всё unit тестирование должно для каждого класса для каждого индекса
 *  вызвать setResult и checkResult.
 *
 *
 * Для реализации нового теста его нужно наследовать от этого и:
 * *Добавить в новый класс поля для хранения текущего результата.
 * Переопределить функцию getTestName, чтобы она возвращала название
 *  теста.
 * *Переопределить функцию getMaxIndex, чтобы она возвращала индекс
 *  последнего результата(количество результатов - 1).
 * *Переопределить функцию setResult, чтобы она записывала результат
 *  выполнения.
 * *Переопределить функцию checkResult, чтобы она проверяла результат
 *  выполнения.
 * *В main.cpp добавить новый тест в массив TESTS.
 * *В main.cpp увеличить на 1 TEST_NUM.
 *
 *  Для примера можно посмотреть RemoveHtmlTagsTest.
 */
class BaseTest {

    BaseTest (BaseTest&);
    BaseTest & operator= (BaseTest&);

protected:

    string lastError;

public:

    BaseTest ();

    const string & getLastError () const;

    virtual bool setResult   (unsigned int index) = 0;
    virtual bool checkResult (unsigned int index) = 0;

    virtual unsigned int getMaxIndex () const = 0;
    virtual const char * getTestName  () const = 0;

    void resetError ();

};
